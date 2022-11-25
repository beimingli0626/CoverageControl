/**
 * A class for Importance Density Function (IDF) for the world
 *
 * TODO: Add functionalities for importance features that are expressed as rectangles, simple polygons, and circles
 **/

#ifndef COVERAGECONTROL_WORLDIDF_H_
#define COVERAGECONTROL_WORLDIDF_H_

#include <vector>
#include <fstream>
#include <iostream>
#define EIGEN_NO_CUDA // Don't use eigen's cuda facility
#include <Eigen/Dense> // Eigen is used for maps

#include "constants.h"
#include "parameters.h"
#include "typedefs.h"
#include "bivariate_normal_distribution.h"
#include "generate_world_map.ch"
#include "map_utils.h"

namespace CoverageControl {

	class WorldIDF {
		private:
			std::vector <BivariateNormalDistribution> normal_distributions_;
			MapType world_map_;

			// The diagonal points are given as input
			// Returns the total importance in a rectangle by summing up for normal distribution

		public:
			WorldIDF() {
				world_map_ = MapType(pWorldMapSize, pWorldMapSize);
			}

			/** Add Normal distribution to world IDF **/
			void AddNormalDistribution(BivariateNormalDistribution const &distribution) {
				normal_distributions_.push_back(distribution);
			}

			/** Integrate each normal distribution over a rectangle (cell).
				*  If the cell is far away, the importance is set to 0
				**/
			double ComputeImportanceRectangle (Point2 const &bottom_left, Point2 const &top_right) const {
				Point2 bottom_right(top_right.x(), bottom_left.y());
				Point2 top_left(Point2(bottom_left.x(), top_right.y()));
				double importance = 0;
				for(auto const &normal_distribution:normal_distributions_) {
					if(normal_distribution.TransformPoint((bottom_left + top_right)/2.).NormSqr() > pTruncationBND * pTruncationBND + pResolution * pResolution) {
						continue;
					}
					importance += normal_distribution.IntegrateQuarterPlane(bottom_left);
					importance -= normal_distribution.IntegrateQuarterPlane(bottom_right);
					importance -= normal_distribution.IntegrateQuarterPlane(top_left);
					importance += normal_distribution.IntegrateQuarterPlane(top_right);
				}
				return importance;
			}

			/** Fills in values of the world_map_ with the total importance for each cell **/
			void GenerateMap() {
				for(size_t i = 0; i < pWorldMapSize; ++i) { // Row (x index)
					double x1 = pResolution * i; // Left x-coordinate of pixel
					double x2 = x1 + pResolution; // Right x-coordinate of pixel
					for(size_t j = 0; j < pWorldMapSize; ++j) { // Column (y index)
						double y1 = pResolution * j; // Lower y-coordinate of pixel
						double y2 = y1 + pResolution; // Upper y-coordinate of pixel
						double importance	= ComputeImportanceRectangle(Point2(x1,y1), Point2(x2,y2));
						if(std::abs(importance) < kEps) {
							importance = 0;
						}
						world_map_(i, j) = importance;
					}
				}
			}

			void GenerateMapCuda() {
				float resolution = (float) pResolution;
				float truncation = (float) pTruncationBND;
				int map_size = (int) pWorldMapSize;

				int num_dists = normal_distributions_.size();
				/* std::cout << "num_dists: " << num_dists << std::endl; */

				BND_Cuda *host_dists = (BND_Cuda*) malloc(num_dists * sizeof(BND_Cuda));

				for(int i = 0; i < num_dists; ++i) {
					auto mean = normal_distributions_[i].GetMean();
					host_dists[i].mean_x = (float)(mean.x());
					host_dists[i].mean_y = (float)(mean.y());
					auto sigma = normal_distributions_[i].GetSigma();
					host_dists[i].sigma_x = (float)(sigma.x());
					host_dists[i].sigma_y = (float)(sigma.y());
					host_dists[i].rho = (float)(normal_distributions_[i].GetRho());
					host_dists[i].scale = (float)(normal_distributions_[i].GetScale());
				}

				float *importance_vec = (float*) malloc(pWorldMapSize * pWorldMapSize * sizeof(float));
				generate_world_map_cuda(host_dists, num_dists, map_size, resolution, truncation, importance_vec);
				/* GenerateMap(); */
				for(size_t i = 0; i < pWorldMapSize; ++i) {
					for(size_t j = 0; j < pWorldMapSize; ++j) {
						/* if(std::abs(world_map_(i, j) - (double) importance_vec[i*pWorldMapSize + j]) > 10e-5) { */
						/* 	std::cout << "Diff: " << i << " " << j <<  " " << world_map_(i, j) << " " << (double) importance_vec[i*pWorldMapSize + j] << std::endl; */
						/* } */
						world_map_(i, j) = (double) (importance_vec[i * pWorldMapSize + j]);
					}
				}

				free(importance_vec);
				free(host_dists);
			}

			/** Write the world map to a file **/
			int WriteWorldMap(std::string const &file_name) const {
				return MapUtils::WriteMap(world_map_, file_name);
			}

			void GetSubWorldMap(Point2 const &pos, int const sensor_size, MapType &submap) const {
				MapUtils::GetSubMap(pos, sensor_size, pWorldMapSize, world_map_, submap);
			}

			double GetMaxValue() const { return world_map_.maxCoeff(); }

			const MapType& GetWorldMap() const { return world_map_; }

	};

} /* namespace CoverageControl */
#endif /* _COVERAGECONTROL_WORLDIDF_H_ */
