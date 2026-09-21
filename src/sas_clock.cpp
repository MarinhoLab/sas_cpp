/*
# Copyright (c) 2016-2023 Murilo Marques Marinho
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
 * @file sas_clock.cpp
 * @brief Timing utilities for control loops.
 */

#include <thread>

#include "marinholab/sas/core/sas_clock.hpp"

namespace marinholab::sas::core
{

Clock::Clock(const double& sampling_time_in_seconds, const bool &enable_statistics):
    Object("marinholab::sas::core::Clock"),
    target_sampling_time_(std::chrono::nanoseconds(int(sampling_time_in_seconds*1e9))),
    overrun_sampling_time_count_(0),
    enable_statistics_(enable_statistics),
    statistics_map_({{{TimeType::Computational,Statistics::Mean},{std::chrono::nanoseconds(0),0}}})
{

}

void Clock::_compute_statistics_()
{
    for(const auto& time_type : {TimeType::Computational,TimeType::EffectiveSampling,TimeType::Idle})
    {
        auto [mean,size] = statistics_map_[{time_type,Statistics::Mean}];
        statistics_map_[{time_type,Statistics::Mean}]={incremental_mean(mean,size,kept_times_map_.at(time_type)),size+1};
    }
}

void Clock::init()
{
    // Get initial time
    time_initial_ = std::chrono::system_clock::now();

    // Initialize all time points
    time_before_sleep_ = time_initial_;
    time_after_sleep_ = time_initial_;
    next_loop_deadline_ = time_initial_;

    overrun_sampling_time_count_ = 0;
}

void Clock::update_and_sleep()
{
    time_before_sleep_ = std::chrono::system_clock::now();

    // The required computation time
    kept_times_map_[TimeType::Computational] = time_before_sleep_ - time_after_sleep_;

    // Define the next deadline
    // It is important to not define an impossible deadline. We also add the number
    // of deadline overruns safely to a counter.
    next_loop_deadline_ += target_sampling_time_;
    while(next_loop_deadline_ < (time_before_sleep_))
    {
        next_loop_deadline_ += target_sampling_time_;
        // Increase counter only if it won't cause an overflow
        if(overrun_sampling_time_count_+1 < std::numeric_limits<long>().max())
        {
            overrun_sampling_time_count_++;
        }
    }

    //Sleep until the deadline
    std::this_thread::sleep_until(next_loop_deadline_);

    time_after_sleep_  = std::chrono::system_clock::now();
    // The time spend sleeping
    kept_times_map_[TimeType::Idle]  = time_after_sleep_ - time_before_sleep_;

    // The effective sampling time
    kept_times_map_[TimeType::EffectiveSampling] = kept_times_map_.at(TimeType::Computational) + kept_times_map_.at(TimeType::Idle);
    if(enable_statistics_)
    {
        _compute_statistics_();
    }
}

std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> Clock::get_initial_time() const
{
    return time_initial_;
}

double Clock::get_sleep_time() const
{
    return kept_times_map_.at(TimeType::Idle).count();
}

std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> Clock::get_last_update_time() const
{
    return time_after_sleep_;
}

double Clock::get_computation_time() const
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(kept_times_map_.at(TimeType::Computational)).count();
}

double Clock::get_desired_thread_sampling_time_sec() const
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(target_sampling_time_).count();
}

double Clock::get_effective_thread_sampling_time_sec() const
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(kept_times_map_.at(TimeType::EffectiveSampling)).count();
}

double Clock::get_elapsed_time_sec() const
{
    std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>(current_time - get_initial_time()).count();
}

void Clock::safe_sleep_seconds(const double& seconds, std::atomic_bool* break_loop)
{
    for(int i=0;i<int(seconds/get_desired_thread_sampling_time_sec()) && !(*break_loop);i++)
    {
        update_and_sleep();
    }
}

void Clock::blocking_sleep_seconds(const double& seconds)
{
    for(int i=0;i<int(seconds/get_desired_thread_sampling_time_sec());i++)
    {
        update_and_sleep();
    }
}

long Clock::get_overrun_count() const
{
    return overrun_sampling_time_count_;
}

double Clock::get_time(const TimeType &time_type) const
{
    return std::chrono::duration_cast<
            std::chrono::duration<double>
            >(kept_times_map_.at(time_type)).count();
}

double Clock::get_statistics(const Statistics& statistics, const TimeType& time_type) const
{
    if(!enable_statistics_)
        throw std::runtime_error("Statistics were not enabled, get_statistics will not return a valid value");
    if(statistics_map_.count({time_type,statistics})<=0)
        throw std::runtime_error("Requested statistics is not available.");

    return std::chrono::duration_cast<
            std::chrono::duration<double>
            >(std::get<0>(statistics_map_.at({time_type,statistics}))).count();
}

};


