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
 * @file sas_core.cpp
 * @brief Implementation of core functions.
 */
#include <marinholab/sas/core/sas_core.hpp>
#include <mutex>

namespace marinholab::sas::core
{

/**
 * @brief concatenate two Eigen::VectorXd.
 * @param a a Eigen::VectorXd.
 * @param b a Eigen::VectorXd.
 * @return the result of the concatenated vectors.
 */
Eigen::VectorXd concatenate(const Eigen::VectorXd& a, const Eigen::VectorXd& b)
{
    return (Eigen::VectorXd (a.size() + b.size()) << a, b).finished();
}

/**
 * @brief concatenate a std::vector of Eigen::VectorXd.
 * @param a an std::vector of Eigen::VectorXd.
 * @return the result of the concatenated vectors.
 */
Eigen::VectorXd concatenate(const std::vector<Eigen::VectorXd>& as)
{
    Eigen::VectorXd b;
    for(const auto& c : as)
    {
        b = concatenate(b, c);
    }
    return b;
}

/**
 * @brief vstack vertically (row-wise) stack two Eigen::MatrixXd.
 * @param A the first Eigen::MatrixXd.
 * @param B the second Eigen::MatrixXd.
 * @return the vstacked Eigen::MatrixXd.
 * @exception a std::range_error if @a A and @a B don't have
 * the same number of columns.
 * @note returns an empty matrix if both arguments are empty
 * or return the other argument of only one of the arguments
 * is empty.
 */
Eigen::MatrixXd vstack(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B)
{
    if((A.size() == 0) && (B.size()==0))
        return Eigen::MatrixXd();
    if(A.size() == 0)
        return B;
    if(B.size() == 0)
        return A;
    if(A.cols()!=B.cols())
        throw std::range_error("vstack needs inputs a and b with the same number of columns.");

    return(Eigen::MatrixXd(A.rows()+B.rows(),A.cols()) << A, B).finished();
}

/**
 * @brief block_diag creates a block diagonal matrix
 * using an input of std::vector<Eigen::MatrixXd>.
 * e.g. if As= [A, B, C],
 * then
 * block_diag(As) =
 * |A 0 0|
 * |0 B 0|
 * |0 0 C|
 * @param As the std::vector<Eigen::MatrixXd> contaning
 * the matrix to form the block diagonal matrix.
 * @return the block diagonal matrix.
 */
Eigen::MatrixXd block_diag(const std::vector<Eigen::MatrixXd>& As)
{
    int rows = 0;
    int cols = 0;
    for(const auto& A : As)
    {
        rows+=A.rows();
        cols+=A.cols();
    }

    Eigen::MatrixXd B = Eigen::MatrixXd::Zero(rows,cols);
    int start_row = 0;
    int start_col = 0;
    for(const auto& A : As)
    {
        const int& this_rows = A.rows();
        const int& this_columns = A.cols();

        B.block(start_row,start_col,this_rows,this_columns) = A;

        start_row+=this_rows;
        start_col+=this_columns;
    }

    return B;
}

/**
 * @brief split splits the input Eigen::VectorXd @a as into a set of subvectors
 * defined by ns.
 * @param as the Eigen::VectorXd to be split.
 * @param ns the sizes of the subvectors.
 * @return an std::vector<Eigen::VectorXd> of the splitted vectors.
 */
std::vector<Eigen::VectorXd> split(const Eigen::VectorXd& a, const std::vector<int>& ns)
{
    std::vector<Eigen::VectorXd> as;
    int n_acc = 0;
    for(const auto& n : ns)
    {
        as.push_back(a.segment(n_acc,n));
        n_acc+=n;
    }
    return as;
}



}
