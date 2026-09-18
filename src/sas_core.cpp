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
 * @brief concatenate two VectorXd.
 * @param a a VectorXd.
 * @param b a VectorXd.
 * @return the result of the concatenated vectors.
 */
VectorXd concatenate(const VectorXd& a, const VectorXd& b)
{
    return (VectorXd (a.size() + b.size()) << a, b).finished();
}

/**
 * @brief concatenate a std::vector of VectorXd.
 * @param a an std::vector of VectorXd.
 * @return the result of the concatenated vectors.
 */
VectorXd concatenate(const std::vector<VectorXd>& as)
{
    VectorXd b;
    for(const auto& c : as)
    {
        b = concatenate(b, c);
    }
    return b;
}

/**
 * @brief vstack vertically (row-wise) stack two MatrixXd.
 * @param A the first MatrixXd.
 * @param B the second MatrixXd.
 * @return the vstacked MatrixXd.
 * @exception a std::range_error if @a A and @a B don't have
 * the same number of columns.
 * @note returns an empty matrix if both arguments are empty
 * or return the other argument of only one of the arguments
 * is empty.
 */
MatrixXd vstack(const MatrixXd& A, const MatrixXd& B)
{
    if((A.size() == 0) && (B.size()==0))
        return MatrixXd();
    if(A.size() == 0)
        return B;
    if(B.size() == 0)
        return A;
    if(A.cols()!=B.cols())
        throw std::range_error("vstack needs inputs a and b with the same number of columns.");

    return(MatrixXd(A.rows()+B.rows(),A.cols()) << A, B).finished();
}

/**
 * @brief block_diag creates a block diagonal matrix
 * using an input of std::vector<MatrixXd>.
 * e.g. if As= [A, B, C],
 * then
 * block_diag(As) =
 * |A 0 0|
 * |0 B 0|
 * |0 0 C|
 * @param As the std::vector<MatrixXd> contaning
 * the matrix to form the block diagonal matrix.
 * @return the block diagonal matrix.
 */
MatrixXd block_diag(const std::vector<MatrixXd>& As)
{
    int rows = 0;
    int cols = 0;
    for(const auto& A : As)
    {
        rows+=A.rows();
        cols+=A.cols();
    }

    MatrixXd B = MatrixXd::Zero(rows,cols);
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
 * @brief split splits the input VectorXd @a as into a set of subvectors
 * defined by ns.
 * @param as the VectorXd to be split.
 * @param ns the sizes of the subvectors.
 * @return an std::vector<VectorXd> of the splitted vectors.
 */
std::vector<VectorXd> split(const VectorXd& a, const std::vector<int>& ns)
{
    std::vector<VectorXd> as;
    int n_acc = 0;
    for(const auto& n : ns)
    {
        as.push_back(a.segment(n_acc,n));
        n_acc+=n;
    }
    return as;
}



}
