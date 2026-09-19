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
 * @file sas_robot_driver_example.hpp
 * @brief Example RobotDriver implementation for testing and demonstration.
 *
 * Provides a simple in-memory RobotDriverExample used as a reference
 * implementation for testing the RobotDriver interfaces without requiring
 * real hardware. Useful for unit tests and examples.
 */
#pragma once

#include <marinholab/sas/core/sas_shutdown_signaler.hpp>
#include <marinholab/sas/core/sas_robot_driver.hpp>
#include <Eigen/Dense>

using namespace Eigen;

namespace marinholab::sas::core
{
/**
 * @brief Configuration for the RobotDriverExample.
 *
 * Holds example-specific parameters such as a human-readable name, initial
 * joint positions and joint limits used by the in-memory example driver.
 */
struct RobotDriverExampleConfiguration
{
    std::string name;
    VectorXd initial_joint_positions;
    std::tuple<VectorXd,VectorXd> joint_limits;
};

/**
 * @brief Simple in-memory RobotDriver implementation for testing.
 *
 * RobotDriverExample is a lightweight, non-hardware driver that implements
 * the RobotDriver interface. It is intended for unit tests and examples and
 * simulates joint state updates using the provided configuration.
 */
 class RobotDriverExample: public RobotDriver
{
protected:
    const RobotDriverExampleConfiguration configuration_;
    VectorXd joint_positions_;

public:
    RobotDriverExample(RobotDriverExample&) = delete;
    RobotDriverExample()=delete;

    /**
     * @brief Construct a RobotDriverExample with configuration
     * @param configuration Example configuration (name, initial positions, joint limits)
     * @param break_loops Optional pointer to an atomic_bool used to break loops
     */
    RobotDriverExample(const RobotDriverExampleConfiguration& configuration, const std::shared_ptr<ShutdownSignaler>& shutdown_signaler_);
    [[deprecated("Use RobotDriverExample(const RobotDriverExampleConfiguration& configuration, const std::shared_ptr<ShutdownSignaler>& shutdown_signaler_) instead.")]]
    RobotDriverExample(const RobotDriverExampleConfiguration& configuration, std::atomic_bool* break_loops);

    /**
     * @brief Get the current joint positions
     * @return Vector of joint positions (radians)
     */
    virtual VectorXd get_joint_positions() override;

    /**
     * @brief Set target joint positions
     * @param set_target_joint_positions_rad Target joint positions in radians
     * @throws std::runtime_error if the input vector has incorrect size
     */
    virtual void set_target_joint_positions(const VectorXd& set_target_joint_positions_rad) override;

    /**
     * @brief Connect the example driver (establish resources)
     */
    virtual void connect() override;

    /**
     * @brief Disconnect the example driver (release resources)
     */
    virtual void disconnect() override;

    /**
     * @brief Initialize the example driver
     */
    virtual void initialize() override;

    /**
     * @brief Deinitialize the example driver
     */
    virtual void deinitialize() override;
};
}
