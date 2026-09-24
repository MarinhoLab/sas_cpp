/*
# Copyright (c) 2016-2026 Murilo Marques Marinho
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
 * @file sas_robot_driver_example.cpp
 * @brief Example implementation of a RobotDriver.
 */
#include <marinholab/sas/core/examples/sas_robot_driver_example.hpp>
#include <iostream>

marinholab::sas::core::RobotDriverExample::RobotDriverExample(const RobotDriverExampleConfiguration &configuration, std::atomic_bool *break_loops):
    RobotDriver(break_loops),
    configuration_(configuration)
{
    set_joint_limits(configuration.joint_limits);
}

marinholab::sas::core::RobotDriverExample::RobotDriverExample(const RobotDriverExampleConfiguration& configuration, const std::shared_ptr<ShutdownSignaler>& shutdown_signaler_):
    RobotDriver(shutdown_signaler_),
    configuration_(configuration)
{
    set_joint_limits(configuration.joint_limits);
}

Eigen::VectorXd marinholab::sas::core::RobotDriverExample::get_joint_positions()
{
    return joint_positions_;
}

void marinholab::sas::core::RobotDriverExample::set_target_joint_positions(const Eigen::VectorXd &set_target_joint_positions_rad)
{
    if(joint_positions_.size() != set_target_joint_positions_rad.size())
        throw std::runtime_error("marinholab::sas::core::RobotDriverExample::set_target_joint_positions invalid size for set_target_joint_positions_rad");
    joint_positions_ = set_target_joint_positions_rad;
}

void marinholab::sas::core::RobotDriverExample::connect()
{
    std::cout << "Connecting to " << configuration_.name << std::endl;
}

void marinholab::sas::core::RobotDriverExample::disconnect()
{
    std::cout << "Disconnecting from " << configuration_.name << std::endl;
}

void marinholab::sas::core::RobotDriverExample::initialize()
{
    std::cout << "Initializing " << configuration_.name << std::endl;
    joint_positions_ = configuration_.initial_joint_positions;
}

void marinholab::sas::core::RobotDriverExample::deinitialize()
{
    std::cout << "Deinitializing " << configuration_.name << std::endl;
}
