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
 * @file sas_core_example.cpp
 * @brief Example testing core utilities.
 */

#include <eigen3/Eigen/Dense>
#include <marinholab/sas/core/sas_core.hpp>

using namespace Eigen;
using namespace marinholab::sas::core;

int main(int,char**)
{
    //VectorXd concatenate(const VectorXd& a, const VectorXd& b);
    VectorXd a(3); a << 1,2,3;
    VectorXd b(3); b << 4,5,6;
    VectorXd c(6); c << 1,2,3,4,5,6;
    assert((concatenate(a,b)==c));

    //VectorXd concatenate(const std::vector<VectorXd>& as);
    auto as = {a,b};
    assert((concatenate(as)==c));

    //MatrixXd vstack(const MatrixXd& A, const MatrixXd& B);
    MatrixXd A(2,2); A << 1,2,3,4;
    MatrixXd B(2,2); B << 5,6,7,8;
    MatrixXd C(4,2); C << A,B;
    assert((vstack(A,B)==C));

    //MatrixXd block_diag(const std::vector<MatrixXd>& As);
    auto As = {A,B};
    MatrixXd C_block_diag(4,4); C_block_diag << A,MatrixXd::Zero(2,2),MatrixXd::Zero(2,2),B;
    assert((block_diag(As)==C_block_diag));

    //std::vector<VectorXd> split(const VectorXd& a, const std::vector<int>& ns);
    VectorXd a_split(10); a_split << 1,2,3,4,5,6,7,8,9,10;
    std::vector<int> ns = {2,5,3};
    auto split_result = split(a_split,ns);
    assert((split_result[0]==(VectorXd(2)<<1,2).finished()));
    assert((split_result[1]==(VectorXd(5)<<3,4,5,6,7).finished()));
    assert((split_result[2]==(VectorXd(3)<<8,9,10).finished()));

    return 0;
}
