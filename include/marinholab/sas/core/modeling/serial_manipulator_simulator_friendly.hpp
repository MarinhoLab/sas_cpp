#pragma once
/*
# Copyright (c) 2016-2025 Murilo Marques Marinho
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
 * @file serial_manipulator_simulator_friendly.hpp
 * @brief A serial-manipulator kinematics model with configurable per-joint
 *        offsets and actuation types.
 */

#include <memory>
#include <stdexcept>
#include <vector>

#include <dqrobotics/robot_modeling/DQ_SerialManipulator.h>

namespace marinholab::sas::core::modeling
{

// The dqrobotics types this model builds on. (The Eigen types used in the
// signatures, e.g. Eigen::MatrixXd/Eigen::VectorXd, are injected into the global namespace
// by the dqrobotics headers.)
using DQ_robotics::DQ;
using DQ_robotics::DQ_SerialManipulator;
using DQ_robotics::DQ_JointType;

/**
 * @brief A serial manipulator whose joints carry explicit pre/post offsets.
 *
 * Each joint contributes a dual-quaternion transformation of the form
 * @c offset_before_ * actuation(q) * offset_after_, which lets the model
 * represent sensor frames and joint offsets that a plain
 * @ref DQ_SerialManipulator would not. Supports both revolute (R) and
 * prismatic (T) joints about any principal axis.
 */
class SerialManipulatorSimulatorFriendly: public DQ_SerialManipulator
{
public:
    /** @brief The actuation type and axis of a single joint. */
    enum class ActuationType{
        RZ, ///< Revolution about the z-axis.
        RY, ///< Revolution about the y-axis.
        RX, ///< Revolution about the x-axis.
        TZ, ///< Translation along the z-axis.
        TY, ///< Translation along the y-axis.
        TX  ///< Translation along the x-axis.
    };
protected:
    std::vector<DQ> offset_before_;
    std::vector<DQ> offset_after_;
    std::vector<ActuationType> actuation_types_;

    DQ _get_w(const int& ith) const;
    DQ _joint_transformation(const double& q, const int& ith) const;
public:


    SerialManipulatorSimulatorFriendly()=delete;
    /**
     * @brief Construct the manipulator.
     *
     * @param offset_before  Per-joint dual-quaternion offset applied before actuation.
     * @param offset_after   Per-joint dual-quaternion offset applied after actuation.
     * @param actuation_types Per-joint actuation type and axis.
     *
     * @throws std::runtime_error if the three vectors do not have equal size.
     */
    SerialManipulatorSimulatorFriendly(const std::vector<DQ>& offset_before,
                                       const std::vector<DQ>& offset_after,
                                       const std::vector<ActuationType>& actuation_types);

    using DQ_SerialManipulator::raw_pose_jacobian;
    using DQ_SerialManipulator::raw_pose_jacobian_derivative;
    using DQ_SerialManipulator::raw_fkm;

    /**
     * @brief Joint types this model can represent.
     * @return A vector containing @c DQ_JointType::REVOLUTE.
     */
    std::vector<DQ_JointType> get_supported_joint_types() const override;

    /**
     * @brief Raw pose Jacobian of the chain up to a given link.
     * @param q_vec         Joint configuration vector.
     * @param to_ith_link   Index of the terminal link.
     * @return An 8 x (to_ith_link+1) dual-quaternion pose Jacobian.
     */
    Eigen::MatrixXd raw_pose_jacobian(const Eigen::VectorXd& q_vec, const int& to_ith_link) const override;
    /**
     * @brief Time derivative of the raw pose Jacobian.
     * @param q             Joint configuration vector.
     * @param q_dot         Joint velocity vector.
     * @param to_ith_link   Index of the terminal link.
     * @return An 8 x (to_ith_link+1) Jacobian-derivative matrix.
     */
    Eigen::MatrixXd raw_pose_jacobian_derivative(const Eigen::VectorXd& q, const Eigen::VectorXd& q_dot, const int& to_ith_link) const override;
    /**
     * @brief Raw forward kinematics of the chain up to a given link.
     * @param q_vec         Joint configuration vector.
     * @param to_ith_link   Index of the terminal link.
     * @return The dual-quaternion pose of the terminal link.
     */
    DQ raw_fkm(const Eigen::VectorXd &q_vec, const int &to_ith_link) const override;
};

}
