#pragma once
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
 * @file sas_clock.hpp
 * @brief Timing utilities for control loops and statistics collection.
 *
 * The Clock class manages loop timing, sleep with
 * early exit, compute elapsed times and collect optional statistics for
 * computational time, idle time and effective sampling time used by
 * control threads.
 */
#include <atomic>
#include <chrono>
#include <map>

#include <marinholab/sas/core/sas_object.hpp>
#include <marinholab/sas/core/sas_core.hpp>

namespace marinholab::sas::core
{

/**
 * @brief Clock utility for timing and statistics in control loops.
 */
class Clock : private marinholab::sas::core::Object
{
public:
    /** @brief Enumeration of time types, see get_time() and get_statistics() */
    enum class TimeType{
        Computational, ///< Time spent between when the instant the previous sleep ended and the instant when the current one started.
        EffectiveSampling, ///< The effective sampling time between sleeps (EffectiveSampling = Computational + Idle).
        Idle ///< Time spent between the instant when the previous sleep started and the instant when it ended.
    };
private:

    // https://stackoverflow.com/questions/65397041/apple-clang-why-can-i-not-create-a-time-point-from-stdchrononanoseconds
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_initial_;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> next_loop_deadline_;

    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_before_sleep_;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> time_after_sleep_;

    const std::chrono::nanoseconds target_sampling_time_;

    std::map<TimeType,std::chrono::nanoseconds> kept_times_map_;

    long overrun_sampling_time_count_;

    const bool enable_statistics_;

    std::map<std::tuple<TimeType,Statistics>,std::tuple<std::chrono::nanoseconds,long>> statistics_map_;
    void _compute_statistics_();

public:

    Clock()=delete;
    Clock(const int&)=delete;

    /**
     * @brief Construct a Clock
     * @param sampling_time_in_seconds Desired sampling time in seconds
     * @param enable_statistics Whether to enable internal statistics collection (default: true)
     */
    explicit Clock(const double& sampling_time_in_seconds, const bool& enable_statistics=true);

    /**
     * @brief Initialize the clock internal state and timers
     */
    void init();

    /**
     * @brief Update internal timing measurements and sleep to respect target sampling time
     */
    void update_and_sleep();

    /**
     * @brief Get elapsed time since last update in seconds
     * @return Elapsed time in seconds
     */
    double get_elapsed_time_sec() const;

    /**
     * @brief Get the initial time point recorded by the clock
     * @return time_point of the initial time
     */
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> get_initial_time() const;

    /**
     * @brief Get the time point of the last update
     * @return time_point of the last update
     */
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> get_last_update_time() const;

    /**
     * @brief Sleep for the specified duration while allowing early exit via break_loop
     * @param seconds Sleep duration in seconds
     * @param break_loop Pointer to an atomic boolean that will interrupt the sleep if set
     */
    void safe_sleep_seconds(const double& seconds, std::atomic_bool* break_loop);

    /**
     * @brief Block the calling thread for the specified duration (no early exit)
     * @param seconds Sleep duration in seconds
     */
    void blocking_sleep_seconds(const double& seconds);

    /**
     * @brief Return the desired sampling time for the thread in seconds
     * @return Desired sampling time in seconds
     */
    double get_desired_thread_sampling_time_sec() const;

    /**
     * @brief Return the number of times the sampling has overrun the target period
     * @return Overrun count
     */
    long get_overrun_count() const;

    /**
     * @brief Get a time value for the provided TimeType
     * @param time_type The TimeType (Computational, EffectiveSampling, Idle)
     * @return Time value in seconds corresponding to time_type
     */
    double get_time(const TimeType& time_type) const;

    /**
     * @brief Get a statistic value for the given statistic type and TimeType
     * @param statistics The statistic to query (see marinholab::sas::core::Statistics)
     * @param time_type The TimeType to which the statistic applies
     * @return The requested statistic value as a double
     * @throws std::runtime_error if statistics collection was not enabled at construction
     *         or if the requested statistic is not available for the provided TimeType.
     */
    double get_statistics(const Statistics &statistics, const TimeType &time_type) const;

    ///Deprecated
    [[deprecated("Use get_time(marinholab::sas::core::Clock::Computational) instead.")]]
    double get_computation_time() const;
    [[deprecated("Use get_time(marinholab::sas::core::Clock::Idle) instead.")]]
    double get_sleep_time() const;
    [[deprecated("Use get_time(marinholab::sas::core::Clock::EffectiveSampling) instead.")]]
    double get_effective_thread_sampling_time_sec() const;
};

}




