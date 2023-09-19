#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <thread>
#include <memory>

#include <CoverageControl/constants.h>
#include <CoverageControl/parameters.h>
#include <CoverageControl/typedefs.h>
#include <CoverageControl/bivariate_normal_distribution.h>
#include <CoverageControl/world_idf.h>
#include <CoverageControl/robot_model.h>
#include <CoverageControl/generate_world_map.ch>
#include <CoverageControlTorch/coverage_system.h>

using namespace CoverageControl;
using namespace CoverageControlTorch;
int main(int argc, char** argv) {
	std::cout << "Processor count: " << std::thread::hardware_concurrency() << std::endl;
	Parameters params;
	if (argc < 5) {
		std::cout << "Usage: ./eval_gnn <parameter_file> <env_dir> <out_dir> <model_dir>" << std::endl;
		return 0;
	}
	/* params.pSensorSize = 16; */
	std::string parameter_file = argv[1];
	params = Parameters(parameter_file);

	std::unique_ptr <CoverageControlTorch::CoverageSystem> env;

	std::string env_dir = argv[2];
	std::string out_dir = argv[3];
	std::cout << std::setprecision(16);
	std::string out_filename = out_dir + "eval.csv";
	std::ofstream out_file(out_filename);
	out_file << std::setprecision(16);


	for (int data_count = 0; data_count < 100; ++data_count) {
		std::cout << "Data count: " << data_count << std::endl;
		std::string dir = "./test/";
		std::string pos_file = env_dir + std::to_string(data_count) + ".pos";
		std::string idf_file = env_dir + std::to_string(data_count) + ".env";
		WorldIDF world_idf(params, idf_file);
		env = std::make_unique<CoverageControlTorch::CoverageSystem> (params, world_idf, pos_file);
		env->InitializeGNNCNN(std::string(argv[4]));
		int num_robots = env->GetNumRobots();
		/* env->PlotInitMap(dir, "init_map"); */
		/* auto goals = oracle.GetGoals(); */
		/* env->PlotMapVoronoi(dir, 0, oracle.GetVoronoi(), oracle.GetGoals()); */
		/* std::cout << "objective value: " << env->GetObjectiveValue() << std::endl; */
		bool cont_flag = true;
		std::cout << "Step: " << 0 << std::endl;
		std::cout << "objective value: " << env->GetObjectiveValue() << std::endl;
		out_file << env->GetObjectiveValue();
		for(int ii = 1; ii < 600; ++ii) {
			env->StepCNNGNN();
			out_file << "," << env->GetObjectiveValue();
			/* std::cout << "Step: " << ii << std::endl; */
			/* bool cont_flag = oracle.Step(); */
			/* auto actions = oracle.GetActions(); */
			/* env->StepActions(actions); */
			if(ii%100 == 0) {
				std::cout << "Step: " << ii << std::endl;
				std::cout << "objective value: " << env->GetObjectiveValue() << std::endl;
				/* env->RecordPlotData(); */
				/* env->PlotMapVoronoi(dir, ii, oracle.GetVoronoi(), oracle.GetGoals()); */
			}
			out_file << std::endl;
		}
	}

	/* auto zero_actions = PointVector(num_robots, Point2(0,0)); */

	/* env->PlotMapVoronoi(dir, 1, oracle.GetVoronoi(), oracle.GetGoals()); */
	/* env->RenderRecordedMap(dir, "CoverageControl_oracle.mp4"); */
	return 0;
}
