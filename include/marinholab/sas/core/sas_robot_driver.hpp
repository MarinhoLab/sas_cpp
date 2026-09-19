#pragma once
/*
# Copyright (c) 2016-2025 Murilo Marques Marinho
#
#    This file is part of sas_robot_driver.
#
#    sas_robot_driver is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    sas_robot_driver is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with sas_robot_driver.  If not, see <https://www.gnu.org/licenses/>.
#
# ################################################################
#
#   Author: Murilo M. Marinho, email: murilomarinho@ieee.org
#
# ################################################################
# Contributors:
#
#   1. Juan Jose Quiroz Omana (juanjose.quirozomana@manchester.ac.uk)
#      Added the Watchdog functionaly initially proposed in
#      https://github.com/SmartArmStack/sas_core/pull/1
*/

/**
 * @file sas_robot_driver.hpp
 * @brief Abstract RobotDriver interface and watchdog support.
 *
 * Declares the RobotDriver abstract base class which concrete drivers must
 * implement to interact with hardware. The header also contains watchdog
 * management used by driver implementations.
 */
#include <atomic>
#include <mutex>
#include <thread>
#include <memory>
#include <chrono>
#include <marinholab/sas/core/sas_shutdown_signaler.hpp>
#include <marinholab/sas/core/sas_clock.hpp>
#include <Eigen/Dense>

using namespace Eigen;

namespace marinholab::sas::core
{
/**
 * @brief Abstract interface for robot hardware drivers.
 *
 * RobotDriver declares the virtual API that concrete drivers must implement
 * to interact with robot hardware.
 */
class RobotDriver
{
protected:
    std::atomic_bool* break_loops_; //Deprecated
    std::shared_ptr<ShutdownSignaler> shutdown_signaler_;
    std::tuple<VectorXd, VectorXd> joint_limits_;
    VectorXd joint_velocities_;
    VectorXd joint_torques_;

    std::unique_ptr<marinholab::sas::core::Clock> clock_;
    std::unique_ptr<std::thread> watchdog_thread_;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_point_from_the_client_;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_point_from_the_server_;
    bool watchdog_status_;
    void _watchdog_thread_function();
    std::mutex mutex_watchdog_;
    double max_acceptable_delay_ = 0.1;
    double watchdog_period_;

    RobotDriver(const std::shared_ptr<ShutdownSignaler>& shutdown_signaler_);
    [[deprecated("Use RobotDriver(const std::shared_ptr<ShutdownSignaler>& shutdown_signaler_) instead.")]]
    RobotDriver(std::atomic_bool* break_loops);

    RobotDriver()=delete;
    RobotDriver(const RobotDriver&)=delete;

    std::exception_ptr watchdog_exception_{nullptr};
    std::mutex watchdog_exception_mutex_;


    std::function<void()> control_loop_callback_;

    public:
    /**
     * @brief Enumeration of optional driver functionalities
     */
    enum class Functionality{
        None=0,
        PositionControl,
        VelocityControl,
        ForceControl,
        Homing,
        ClearPositions,
        Watchdog
    };

    /**
     * @brief Virtual destructor for RobotDriver
     */
    virtual ~RobotDriver();

    /**
     * @brief Get current joint positions
     * @return Vector of joint positions (radians)
     */
    virtual VectorXd get_joint_positions() = 0;

    /**
     * @brief Set target joint positions
     * @param set_target_joint_positions_rad Target joint positions (radians)
     */
    virtual void set_target_joint_positions(const VectorXd& set_target_joint_positions_rad) = 0;

    /**
     * @brief Get current joint velocities
     * @return Vector of joint velocities
     * @throws std::runtime_error if the default implementation is called (not implemented by derived driver)
     */
    virtual VectorXd get_joint_velocities();

    /**
     * @brief Set target joint velocities
     * @param set_target_joint_velocities Target joint velocities
     * @throws std::runtime_error if the default implementation is called (not implemented by derived driver)
     */
    virtual void set_target_joint_velocities(const VectorXd& set_target_joint_velocities);

    /**
     * @brief Get current joint torques
     * @return Vector of joint torques
     * @throws std::runtime_error if the default implementation is called (not implemented by derived driver)
     */
    virtual VectorXd get_joint_torques();

    /**
     * @brief Set target joint torques
     * @param set_target_joint_torques Target joint torques
     * @throws std::runtime_error if the default implementation is called (not implemented by derived driver)
     */
    virtual void set_target_joint_torques(const VectorXd& set_target_joint_torques);

    /**
     * @brief Get joint limits (min, max)
     * @return Tuple of (min_limits, max_limits)
     */
    virtual std::tuple<VectorXd, VectorXd> get_joint_limits();

    /**
     * @brief Set joint limits (min, max)
     * @param joint_limits Tuple of (min_limits, max_limits)
     */
    virtual void set_joint_limits(const std::tuple<VectorXd, VectorXd>& joint_limits);

    /**
     * @brief Start the watchdog thread with the given period
     * @param period Watchdog period as nanoseconds
     */
    void watchdog_start(const std::chrono::nanoseconds& period);

    /**
     * @brief Trigger the watchdog with timestamps from client/server and the current status
     * @param time_point_from_the_client Time point provided by the client
     * @param time_point_from_the_server Time point provided by the server
     * @param status Current watchdog status flag
     */
    void watchdog_trigger(const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& time_point_from_the_client,
                          const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& time_point_from_the_server,
                          const bool& status);

    /**
     * @brief Set the maximum acceptable delay for the watchdog (seconds)
     * @param max_acceptable_delay Maximum delay in seconds
     */
    void watchdog_set_maximum_acceptable_delay(const double& max_acceptable_delay);

    /**
     * @brief Check for exceptions thrown by the watchdog thread and rethrow if present
     * @throws std::runtime_error if the watchdog thread detected a timing or status error
     * @throws std::exception rethrows any exception captured from the watchdog thread
     */
    void check_for_watchdog_exceptions();

    /**
     * @brief Connect to the underlying robot/hardware
     */
    virtual void connect()=0;

    /**
     * @brief Disconnect from the underlying robot/hardware
     */
    virtual void disconnect()=0;

    /**
     * @brief Initialize the driver resources
     */
    virtual void initialize()=0;

    /**
     * @brief Deinitialize the driver resources
     */
    virtual void deinitialize()=0;


    /**
     * @brief Set the control loop callback function
     * @param callback The callback function to be executed in the control loop
     *
     */
    void set_control_loop_callback(std::function<void()> callback);


    /**
     * @brief Execute the control loop callback if it has been set
     *
     * This method should be called by RobotDriverROS or any other class
     * that runs the control loop. It will execute the callback that was
     * set by the concrete RobotDriver implementation.
     */
    void execute_control_loop_callback();


    /**
     * @brief Check if a control loop callback has been set
     * @return true if a callback has been set, false otherwise
     */
    bool control_loop_callback_is_set();
};
}



