/*
# Copyright (c) 2016-2026 Murilo Marques Marinho
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
# ################################################################# Contributors:
#
#   1. Juan Jose Quiroz Omana (juanjose.quirozomana@manchester.ac.uk)
#      Added the Watchdog functionality initially proposed in
#      https://github.com/SmartArmStack/sas_core/pull/1
#*/

/**
 * @file sas_robot_driver.cpp
 * @brief Implementation of the RobotDriver base class and watchdog.
 */


#include <marinholab/sas/core/sas_robot_driver.hpp>
#include <marinholab/sas/core/sas_clock.hpp>

namespace marinholab::sas::core
{

RobotDriver::RobotDriver(std::atomic_bool *break_loops):
    break_loops_(break_loops),
    shutdown_signaler_(std::make_shared<ShutdownSignaler>(break_loops))
{

}

RobotDriver::RobotDriver(const std::shared_ptr<ShutdownSignaler>& shutdown_signaler):
    break_loops_(nullptr),
    shutdown_signaler_(shutdown_signaler)
{
}

RobotDriver::~RobotDriver()
{
    if (watchdog_thread_ && watchdog_thread_->joinable()) {
        shutdown_signaler_->shutdown(); //To force the thread to shutdown if it hasn't already done so
        watchdog_thread_->join();
    }
}

VectorXd RobotDriver::get_joint_velocities()
{
    throw std::runtime_error("Not implemented yet.");
}

void RobotDriver::set_target_joint_velocities(const VectorXd&)
{
    throw std::runtime_error("Not implemented yet.");
}

VectorXd RobotDriver::get_joint_torques()
{
    throw std::runtime_error("Not implemented yet.");
}

void RobotDriver::set_target_joint_torques(const VectorXd &)
{
    throw std::runtime_error("Not implemented yet.");
}

std::tuple<VectorXd, VectorXd> RobotDriver::get_joint_limits()
{
    return joint_limits_;
}

void RobotDriver::set_joint_limits(const std::tuple<VectorXd, VectorXd> &joint_limits)
{
    joint_limits_ = joint_limits;
}


/**
 * @brief RobotDriver::_watchdog_thread_function throws an exception if the elapsed time since the last trigger
 *        exceeds the specified period.
 */
void RobotDriver::_watchdog_thread_function()
{
    const double thread_freq =1.0/clock_->get_desired_thread_sampling_time_sec();
    clock_->init();
    while(!shutdown_signaler_->should_shutdown())
    {
        try{
            std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
            double elapsed_time;
            double elapsed_time_same_clock;
            bool wstatus;
            {
                std::scoped_lock lock(mutex_watchdog_);
                elapsed_time            = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - time_point_from_the_client_).count();
                elapsed_time_same_clock = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - time_point_from_the_server_).count();
                wstatus = watchdog_status_;
            }

            double clock_difference = std::abs(elapsed_time - elapsed_time_same_clock);
            if (clock_difference > max_acceptable_delay_)
                throw std::runtime_error(
                    std::string("RobotDriver:: The watchdog signal is delayed, or the clocks between the client and server are out of synch! ") +
                    "Watchdog signal delay: " + std::to_string(1000*clock_difference) +"ms."
                    );



            if (elapsed_time_same_clock  > watchdog_period_)
                throw std::runtime_error(
                    std::string("RobotDriver:: The watchdog signal was lost! ") +
                    "The elapsed time was " + std::to_string(elapsed_time_same_clock) +
                    " seconds, but the period is " + std::to_string(watchdog_period_) + ". There was a watchdog signal delay of " + std::to_string(1000*clock_difference) +
                    "ms." +
                    "The watchdog thread runs at " + std::to_string(thread_freq)+ "Hz."
                    );

            if(!wstatus)
                throw std::runtime_error("RobotDriver:: The watchdog status is false!");
            clock_->update_and_sleep();
        }catch (...)
        {
            std::scoped_lock lock(watchdog_exception_mutex_);
            watchdog_exception_ = std::current_exception();
        }
    }
}

/**
 * @brief RobotDriver::watchdog_start starts the watchdog thread
 * @param period The period of time.
 */
void RobotDriver::watchdog_start(const std::chrono::nanoseconds& period)
{
    if (!clock_)
    {
        watchdog_period_ =  std::chrono::duration_cast<std::chrono::duration<double>>(period).count();
        // If the watchdog period is 1 second, the watchdog thread control is going to check five times per second.
        clock_ = std::make_unique<marinholab::sas::core::Clock>(watchdog_period_/5.0);
    }

    if (!watchdog_thread_)
        watchdog_thread_ = std::make_unique<std::thread>(&RobotDriver::_watchdog_thread_function, this);

}


/**
 * @brief RobotDriver::watchdog_trigger updates the trigger signal. 
 * @param time_point_from_the_client This time point corresponds to the moment the signal was sent, as recorded by the client computer's clock.
 * @param time_point_from_the_server The time point when the watchdog signal was received. This time point uses
 *                                 the computer's clock on which the server (robot) is running.
 * @param status The desired status. If false, the driver is going to stop.
 */
void RobotDriver::watchdog_trigger(const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& time_point_from_the_client,
                                   const std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>& time_point_from_the_server,
                                   const bool& status)
{
    {
        std::scoped_lock lock(mutex_watchdog_);
        time_point_from_the_client_    = time_point_from_the_client;
        time_point_from_the_server_    = time_point_from_the_server;
        watchdog_status_ = status;
    }
}


/**
 * @brief RobotDriver::watchdog_set_maximum_acceptable_delay sets the maximum acceptable clock skew to check the synchronization or
 *                          potential delays between the time point of watchdog signal sent by the client and
 *                          the time point when the watchdog signal was received.
 * @param max_acceptable_delay
 */
void RobotDriver::watchdog_set_maximum_acceptable_delay(const double &max_acceptable_delay)
{
    max_acceptable_delay_ = max_acceptable_delay;
}

/**
 * @brief RobotDriver::check_for_watchdog_exceptions this method rethrows any exception thrown in the watchdog thread control loop.
 */
void RobotDriver::check_for_watchdog_exceptions()
{
    std::scoped_lock lock(watchdog_exception_mutex_);
    if (watchdog_exception_)
        std::rethrow_exception(watchdog_exception_);
}

/**
 * @brief RobotDriver::set_control_loop_callback
 * @param callback
 */
void RobotDriver::set_control_loop_callback(std::function<void()> callback)
{
    control_loop_callback_ = std::move(callback);
}


/**
 * @brief Execute the control loop callback if it has been set
 *
 * This method should be called by RobotDriverROS or any other class
 * that runs the control loop. It will execute the callback that was
 * set by the concrete RobotDriver implementation.
 */
void RobotDriver::execute_control_loop_callback()
{
    if (control_loop_callback_)
    {
        try
        {
            control_loop_callback_();
        }
        catch(const std::exception& e)
        {
            throw std::runtime_error("Control loop callback exception: " + std::string(e.what()));
        }
    }
}

/**
 * @brief Check if a control loop callback has been set
 * @return true if a callback has been set, false otherwise
 */
bool RobotDriver::control_loop_callback_is_set()
{
    return static_cast<bool>(control_loop_callback_);
}


}
