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

#include <cassert>
#include <Eigen/Dense>
#include <marinholab/sas/core/sas_core.hpp>

using namespace marinholab::sas::core;

int main(int,char**)
{
    //Eigen::VectorXd concatenate(const Eigen::VectorXd& a, const Eigen::VectorXd& b);
    Eigen::VectorXd a(3); a << 1,2,3;
    Eigen::VectorXd b(3); b << 4,5,6;
    Eigen::VectorXd c(6); c << 1,2,3,4,5,6;
    assert((concatenate(a,b)==c));

    //Eigen::VectorXd concatenate(const std::vector<Eigen::VectorXd>& as);
    auto as = {a,b};
    assert((concatenate(as)==c));

    //Eigen::MatrixXd vstack(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B);
    Eigen::MatrixXd A(2,2); A << 1,2,3,4;
    Eigen::MatrixXd B(2,2); B << 5,6,7,8;
    Eigen::MatrixXd C(4,2); C << A,B;
    assert((vstack(A,B)==C));

    //Eigen::MatrixXd block_diag(const std::vector<Eigen::MatrixXd>& As);
    auto As = {A,B};
    Eigen::MatrixXd C_block_diag(4,4); C_block_diag << A,Eigen::MatrixXd::Zero(2,2),Eigen::MatrixXd::Zero(2,2),B;
    assert((block_diag(As)==C_block_diag));

    //std::vector<Eigen::VectorXd> split(const Eigen::VectorXd& a, const std::vector<int>& ns);
    Eigen::VectorXd a_split(10); a_split << 1,2,3,4,5,6,7,8,9,10;
    std::vector<int> ns = {2,5,3};
    auto split_result = split(a_split,ns);
    assert((split_result[0]==(Eigen::VectorXd(2)<<1,2).finished()));
    assert((split_result[1]==(Eigen::VectorXd(5)<<3,4,5,6,7).finished()));
    assert((split_result[2]==(Eigen::VectorXd(3)<<8,9,10).finished()));

    return 0;
}
