/*
# Copyright (c) 2022-2023 Murilo Marques Marinho
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
 * @file sas_core.hpp
 * @brief Core numerical utilities used across the project.
 *
 * This header contains lightweight, frequently used functions such as
 * incremental mean computation, vector/matrix concatenation, block-diagonal
 * assembly and vector splitting utilities. These utilities are intended to be
 * header-only and efficient for use in real-time control loops.
 */
#pragma once

#include <Eigen/Dense>


namespace marinholab::sas::core
{

/**
 * @brief Statistical measures supported by sas_core utilities.
 *
 * Enumeration of lightweight statistics that classes and functions may
 * compute (for example, Clock statistics). Add new entries here as new
 * statistics are supported.
 */
enum class Statistics{
    Mean
};

template<class T>
/**
 * @brief incremental_mean a simple implementation of incremental mean, for the many cases in which
 * keeping a vector of all values would be impractical.
 * @param current_mean the current value of the mean.
 * @param current_number_of_samples the current number of samples, not considering the new_sample.
 * @param new_sample the new sample that will change the mean.
 * @throws std::range_error if current_number_of_samples is negative.
 * @return the incremental mean, considering the new_sample.
 */
constexpr T incremental_mean(const T &current_mean, const int &current_number_of_samples, const T &new_sample)
{
    if(current_number_of_samples<0)
        throw std::range_error("incremental_mean::current_number_of_samples should be larger than 0");
    return (current_mean * current_number_of_samples + new_sample)/(current_number_of_samples+1);
}

/**
 * @brief Concatenate two vectors by appending b after a.
 * @param a First vector.
 * @param b Second vector.
 * @return Concatenated vector containing all elements of a followed by b.
 */
Eigen::VectorXd concatenate(const Eigen::VectorXd& a, const Eigen::VectorXd& b);

/**
 * @brief Concatenate a list of vectors into a single vector.
 * @param as Vector of vectors to concatenate in order.
 * @return Concatenated vector containing the elements of each input vector in order.
 */
Eigen::VectorXd concatenate(const std::vector<Eigen::VectorXd>& as);

/**
 * @brief Stack two matrices vertically (A above B).
 * @param A Top matrix.
 * @param B Bottom matrix.
 * @return Matrix formed by stacking A on top of B. Columns must match.
 * @throws std::range_error if A and B have different numbers of columns.
 */
Eigen::MatrixXd vstack(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B);

/**
 * @brief Create a block-diagonal matrix from a list of matrices.
 * @param As Vector of matrices to place on the block diagonal.
 * @return Block-diagonal matrix containing the input matrices along its diagonal.
 */
Eigen::MatrixXd block_diag(const std::vector<Eigen::MatrixXd>& As);

/**
 * @brief Split a vector into pieces with sizes specified by ns.
 * @param a Vector to split.
 * @param ns Sizes of each piece; their sum must equal a.size().
 * @return Vector containing the split Eigen::VectorXd pieces.
 */
std::vector<Eigen::VectorXd> split(const Eigen::VectorXd& a, const std::vector<int>& ns);

}
