/*
# Copyright (c) 2022-2026 Murilo Marques Marinho
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
#   Author: Juan Jose Quiroz Omana, email: juanjose.quirozomana@manchester.ac.uk
#
# ################################################################*/

#pragma once
#include <marinholab/sas/core/sas_core.hpp>
#include <marinholab/sas/core/sas_clock.hpp>
#include <functional>
#include <thread>
#include <atomic>
#include <string>
#include <mutex>

namespace marinholab::sas::core
{

class ThreadManager
{
public:
    /**
     * @brief Abstract priority levels for thread_manager threads.
     *
     * These levels are mapped to concrete Linux scheduling policies and
     * priority/nice values in apply_priority(). The enum values themselves
     * (0, 10, 50, 80, 90, 99) are NOT passed directly to the OS in most cases —
     * they exist purely to establish a monotonic ordering (LOWEST < ...
     * CRITICAL) that is easy to read and compare in application code. The
     * actual OS-level values used for each level are documented per-enumerator
     * below, and are applied in sas_thread_manager.cpp.
     *
     * @details Linux scheduling background:
     *          - SCHED_OTHER (the default, non-realtime policy) does not use
     *            sched_priority (it must be 0); differentiation between
     *            SCHED_OTHER threads is done via the nice value, range -20
     *            (highest priority) to 19 (lowest), see setpriority(2).
     *          - SCHED_FIFO and SCHED_RR (realtime policies) use
     *            sched_priority directly, range 1 (low) to 99 (high) on
     *            Linux, see sched(7). Using these policies requires root or
     *            the CAP_SYS_NICE capability.
     *
     * @note The specific numeric choices below (nice 19/10/0, sched_priority
     *       80/90/99) are this library's own convention, not values mandated
     *       by POSIX or Linux. Only the *range* (nice: -20..19, real-time
     *       sched_priority: 1..99) and the *meaning of SCHED_OTHER's priority
     *       field being 0* are OS-defined. CRITICAL uses 99 specifically
     *       because that is the maximum legal sched_priority for SCHED_FIFO/
     *       SCHED_RR on Linux (sched_get_priority_max()), i.e. "as high as
     *       the OS allows." HIGH (80) and REALTIME (90) are spaced below
     *       that ceiling to preserve headroom and a clear ordering, not
     *       because those exact numbers carry any special OS meaning.
     *
     * @warning Real-time priorities are Linux static priorities: within a
     *          given policy, EQUAL priorities are round-robined (SCHED_RR)
     *          or run strictly FIFO (SCHED_FIFO) — see sched(7). Two threads
     *          both at REALTIME or CRITICAL can starve each other under
     *          SCHED_FIFO if neither blocks or yields.
     *
     * @see apply_priority() for the concrete Linux policy/value mapping.
     */
    enum class PRIORITY {
        LOWEST     = 0,  ///< SCHED_OTHER, nice 19 (least favorable, lowest priority)
        BACKGROUND = 10, ///< SCHED_OTHER, nice 10
        NORMAL     = 50, ///< SCHED_OTHER, nice 0 (default OS priority)
        HIGH       = 80, ///< SCHED_RR, sched_priority 80 (requires root/CAP_SYS_NICE)
        REALTIME   = 90, ///< SCHED_FIFO, sched_priority 90 (requires root/CAP_SYS_NICE)
        CRITICAL   = 99  ///< SCHED_FIFO, sched_priority 99 — the OS-defined maximum
    };


private:
    // Thread optimization parameters
    PRIORITY priority_{PRIORITY::NORMAL};
    int cpu_core_{-1};
    void apply_priority();
    void apply_cpu_affinity();
    mutable std::mutex mutex_;


protected:
    marinholab::sas::core::Clock clock_;
    std::function<void()> loop_callback_;
    std::thread thread_;
    std::string thread_name_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stop_requested_{false};

protected:
    void run();
public:
    ~ThreadManager();

    ThreadManager(const ThreadManager&) = delete;            // Copy constructor
    ThreadManager& operator=(const ThreadManager&) = delete; // Copy assignment
    ThreadManager(ThreadManager&&) = delete;                 // Move constructor
    ThreadManager& operator=(ThreadManager&&) = delete;      // Move assignment

    ThreadManager(const std::string& thread_name,
                   const double& period,
                   std::function<void()> callback,
                   PRIORITY priority = PRIORITY::NORMAL,
                   int cpu_core = -1);


    const marinholab::sas::core::Clock& get_clock() const;

    // Lifecycle
    void start();
    void stop();
    bool is_running() const;


    // Getters
    std::string get_thread_name() const;
    PRIORITY get_priority() const;
    int get_cpu_core() const;
    double get_period() const;
};

}

