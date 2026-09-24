/*
# Copyright (c) 2016-2023 Murilo Marques Marinho
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
 * @file eigen3_std_conversions.cpp
 * @brief Conversion functions between Eigen, STL containers, and DQ.
 */
#include<marinholab/sas/core/eigen3_std_conversions.hpp>

namespace marinholab::sas::core
{

std::vector<double> vectorxd_to_std_vector_double(const Eigen::VectorXd& vectorxd)
{
    std::vector<double> vec(vectorxd.data(), vectorxd.data() + vectorxd.rows() * vectorxd.cols());
    return vec;
}

Eigen::VectorXd std_vector_double_to_vectorxd(std::vector<double> std_vector_double)
{
    double* ptr = &std_vector_double[0];
    Eigen::Map<Eigen::VectorXd> vec(ptr,std_vector_double.size()); //We need access to the pointer here so we cannot use const ref
    return vec;
}

DQ std_vector_double_to_dq(const std::vector<double> &std_vector_double)
{
    return DQ(std_vector_double_to_vectorxd(std_vector_double));
}

std::vector<int> vectorxi_to_std_vector_int(const Eigen::VectorXi &vectorxi)
{
    std::vector<int> vec(vectorxi.data(), vectorxi.data() + vectorxi.rows() * vectorxi.cols());
    return vec;
}

Eigen::VectorXi std_vector_int_to_vectorxi(std::vector<int> std_vector_int)
{
    int* ptr = &std_vector_int[0];
    Eigen::Map<Eigen::VectorXi> vec(ptr,std_vector_int.size()); //We need access to the pointer here so we cannot use const ref
    return vec;
}

}


