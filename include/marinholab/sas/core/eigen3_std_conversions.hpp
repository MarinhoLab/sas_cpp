#pragma once
/*
# Copyright (c) 2016-2020 Murilo Marques Marinho
#
#    This file is part of sas_core.
#
#    sas_core is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    sas_core is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with sas_core.  If not, see <https://www.gnu.org/licenses/>.
#
# ################################################################
#
#   Author: Murilo M. Marinho, email: murilomarinho@ieee.org
#
# ################################################################*/

/**
 * @file eigen3_std_conversions.hpp
 * @brief Conversions between Eigen, std::vector and DQ types.
 *
 * Small utility functions to convert between Eigen's Vector/Matrix types,
 * std::vector and DQ (dual quaternion) representations used across the
 * project.
 */
#include<vector>
#include<eigen3/Eigen/Dense>
#include<dqrobotics/DQ.h>

using namespace Eigen;
using namespace DQ_robotics;

namespace marinholab::sas::core
{
/**
 * @brief Convert an Eigen::VectorXd to a std::vector<double>.
 * @param vectorxd Source Eigen vector.
 * @return std::vector<double> containing the same elements in order.
 */
std::vector<double> vectorxd_to_std_vector_double(const VectorXd& vectorxd);

/**
 * @brief Convert an Eigen::VectorXi to a std::vector<int>.
 * @param vectorxi Source Eigen integer vector.
 * @return std::vector<int> containing the same elements in order.
 */
std::vector<int>    vectorxi_to_std_vector_int(const VectorXi& vectorxi);

/**
 * @brief Convert a std::vector<double> to an Eigen::VectorXd.
 * @param std_vector_double Source std::vector<double>.
 * @return VectorXd containing the same elements.
 */
VectorXd            std_vector_double_to_vectorxd(std::vector<double> std_vector_double);

/**
 * @brief Convert a std::vector<int> to an Eigen::VectorXi.
 * @param std_vector_int Source std::vector<int>.
 * @return VectorXi containing the same elements.
 */
VectorXi            std_vector_int_to_vectorxi(std::vector<int> std_vector_int);

/**
 * @brief Convert a std::vector<double> to a DQ (dual quaternion) object.
 * @param std_vector_double Source std::vector<double> containing the DQ coefficients.
 * @return DQ constructed from the provided coefficients.
 */
DQ                  std_vector_double_to_dq(const std::vector<double>& std_vector_double);
}

