/*
 * This file is part of the CoverageControl library
 *
 * Author: Saurav Agarwal
 * Contact: sauravag@seas.upenn.edu, agr.saurav1@gmail.com
 * Repository: https://github.com/KumarRobotics/CoverageControl
 *
 * The CoverageControl library is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * The CoverageControl library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with CoverageControl library. If not, see <https://www.gnu.org/licenses/>.
 */

/*!
 * \file constants.h
 * \brief Constants for the CoverageControl library
 */

#ifndef COVERAGECONTROL_CONSTANTS_H_
#define COVERAGECONTROL_CONSTANTS_H_

#include <cmath>
#include <limits>

/*!
 * \namespace CoverageControl
 * \brief Namespace for the CoverageControl library
 */
namespace CoverageControl {

	double const kEps = 1e-10; /*!< Epsilon for double comparison */
	double const kLargeEps = 1e-4; /*!< Large epsilon for double comparison */
	double const kSqrt2 = std::sqrt(2); /*!< Square root of 2 */
	double const kOneBySqrt2 = 1./std::sqrt(2); /*!< 1 by square root of 2 */
	double const kInfD = std::numeric_limits<double>::infinity(); /*!< Infinity for double */
	constexpr auto kMaxPrecision {std::numeric_limits<long double>::digits10 + 1};  /*!< Maximum precision for long double */

} /* namespace CoverageControl */
#endif /* COVERAGECONTROL_CONSTANTS_H_ */

