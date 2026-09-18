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

#include <iostream>
#include <marinholab/sas/core/sas_thread_manager.hpp>
#include <stdexcept>
#include <cstring>
#include <cassert>

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#endif

namespace marinholab::sas::core
{

/**
 * @brief Constructor for ThreadManager
 *
 * Creates a thread manager that executes a callback function periodically
 * with configurable priority and CPU affinity.
 *
 * @param thread_name Name of the thread for identification and debugging.
 *                    On Linux, names are truncated to 15 characters.
 * @param period Period in seconds between callback executions.
 *               Must be greater than 0.0.
 * @param callback Function to call periodically. The callback should be
 *                 thread-safe and should not block for extended periods
 *                 to avoid missing deadlines.
 * @param priority Thread priority level (default: PRIORITY::NORMAL).
 *                 Higher priorities get more CPU time:
 *                 - LOWEST (0): SCHED_OTHER, nice 19
 *                 - BACKGROUND (10): SCHED_OTHER, nice 10
 *                 - NORMAL (50): SCHED_OTHER, nice 0
 *                 - HIGH (80): SCHED_RR, priority 80 (requires root)
 *                 - REALTIME (90): SCHED_FIFO, priority 90 (requires root)
 *                 - CRITICAL (99): SCHED_FIFO, priority 99 (requires root)
 * @param cpu_core CPU core to pin the thread to (-1 = no affinity, default).
 *                 Pinning to a specific core improves cache locality and
 *                 reduces context switching for real-time applications.
 *
 * @throws std::invalid_argument if period <= 0.0.
 *
 * @note Real-time priorities (HIGH, REALTIME, CRITICAL) require root
 *       privileges or CAP_SYS_NICE capability.
 *
 * @warning The callback must NEVER call start() or stop() on this same
 *          ThreadManager instance, whether directly or indirectly (e.g.
 *          through a nested function call). start() and stop() run under
 *          an internal mutex that is held while the loop thread is joined;
 *          calling either from within the callback (which runs on that
 *          same loop thread) will deadlock, since the thread would be
 *          waiting to acquire a mutex held by the very call that is
 *          waiting for the thread to finish. A stop() call from the
 *          callback would additionally attempt to join the thread from
 *          within itself, which is undefined behavior regardless of the
 *          mutex. If the callback needs to end the loop, it should signal
 *          that externally (e.g. via a flag it exposes) and let a
 *          different thread call stop().
 *
 * @see PRIORITY
 * @see start()
 * @see apply_priority()
 * @see apply_cpu_affinity()
 *
 * @code
 * // Create a normal priority thread with no CPU affinity
 * ThreadManager worker("Worker", 0.01, []() { do_work(); });
 *
 * // Create a real-time thread pinned to CPU core 2
 * ThreadManager control(
 *     "Control",
 *     0.001,
 *     []() { compute_control(); },
 *     ThreadManager::PRIORITY::CRITICAL,
 *     2
 * );
 * @endcode
 */
ThreadManager::ThreadManager(const std::string& thread_name,
                               const double& period,
                               std::function<void()> callback,
                               PRIORITY priority,
                               int cpu_core)
    :priority_{priority},
    cpu_core_{cpu_core},
    clock_{period},
    loop_callback_{std::move(callback)},
    thread_name_{thread_name}
{
    if (period <= 0.0) {
        throw std::invalid_argument("ThreadManager: period must be positive");
    }
}

/**
 * @brief Get the CPU core that this thread is pinned to.
 * @return CPU core number, or -1 if no affinity is set.
 */
int ThreadManager::get_cpu_core() const
{
    return cpu_core_;
}

/**
 * @brief Get the desired loop period (sampling time).
 * @return Period in seconds.
 * @details Returns the target period configured at construction time.
 */
double ThreadManager::get_period() const
{
    return clock_.get_desired_thread_sampling_time_sec();
}

/**
 * @brief Get the priority level of this thread.
 * @return The PRIORITY enum value.
 */
ThreadManager::PRIORITY ThreadManager::get_priority() const
{
    return priority_;
}

/**
 * @brief ThreadManager::~ThreadManager destructor of the class
 */
ThreadManager::~ThreadManager()
{
    stop();
}


/**
 * @brief Start the thread execution
 */
void ThreadManager::start()
{
    // Lock enabled
    std::scoped_lock lock(mutex_);

    if (!running_.exchange(true))
    {
        stop_requested_ = false;
        clock_.init();
        thread_ = std::thread(&ThreadManager::run, this);
    }
    // Lock released automatically when scoped_lock goes out of scope
}

/**
 * @brief Stop the thread execution and wait for completion
 */
void ThreadManager::stop()
{
    std::scoped_lock lock(mutex_);

    if (running_.load())
    {
        //This is semantically equivalent to stop_requested_ = true
        //Just to emphasize that it is an atomic variable.
        stop_requested_.store(true);

        if (thread_.joinable()) {
            if (std::this_thread::get_id() == thread_.get_id()) {
                std::cerr << "Fatal: ThreadManager::stop() called from its own "
                             "callback thread ('" << thread_name_
                          << "'); this would deadlock." << std::endl;
            }
            thread_.join();
        }
        running_.store(false);
    }
}


/**
 * @brief Check if the thread is currently running
 * @return true if running, false otherwise
 */
bool ThreadManager::is_running() const
{
    // Returns the current value of running_ in thread-safe way.
    return running_.load();
}


/**
 * @brief Get the thread name
 * @return Thread name string
 */
std::string ThreadManager::get_thread_name() const
{
    return thread_name_;
}


/**
 * @brief Main thread entry function
 */
void ThreadManager::run()
{
    // Apply thread optimizations
    apply_priority();
    apply_cpu_affinity();


    // Set thread name (if supported by platform)
#ifdef __linux__
    // pthread_setname_np has a 16 character limit (including null terminator)
    // see https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html
    std::string truncated_name = thread_name_;
    if (truncated_name.length() > 15) {
        truncated_name.resize(15);
    }
    pthread_setname_np(pthread_self(), truncated_name.c_str());
#endif

    // Main loop
    while (!stop_requested_.load()) {
        // Execute callback
        if (loop_callback_) {
            loop_callback_();
        }
        clock_.update_and_sleep();
    }
}

/**
 * @brief Apply the configured priority and scheduling policy to the thread.
 *
 * Sources: https://man7.org/linux/man-pages/man7/sched.7.html
 *          https://man7.org/linux/man-pages/man2/setpriority.2.html
 *          https://www.ibm.com/docs/en/zos/3.2.0?topic=functions-setpriority-set-process-scheduling-priority
 *          The Linux Programming Interface by Michael Kerrisk (Chapter 35 PROCESS PRIORITIES AND SCHEDULING)
 *
 *
 * This method sets the thread's scheduling priority and policy based on the
 * PRIORITY enum value configured at construction time. It uses Linux's
 * pthread_setschedparam API to apply the settings.
 *
 * @details The method maps the abstract PRIORITY levels to specific Linux
 *          scheduler policies and priority values:
 *
 * | PRIORITY Level   | Linux Policy | Priority Value | Description |
 * |------------------|--------------|----------------|-------------|
 * | LOWEST           | SCHED_OTHER  | 0 (nice 19)    | Low priority background tasks |
 * | BACKGROUND       | SCHED_OTHER  | 0 (nice 10)    | Background processing |
 * | NORMAL           | SCHED_OTHER  | 0 (nice 0)     | Standard scheduling |
 * | HIGH             | SCHED_RR     | 80             | Round-robin real-time |
 * | REALTIME         | SCHED_FIFO   | 90             | FIFO real-time |
 * | CRITICAL         | SCHED_FIFO   | 99             | Highest priority (max) |
 *
 * @note This method is only available on Linux systems.
 * @note Real-time policies (SCHED_RR and SCHED_FIFO) require root privileges
 *       or the CAP_SYS_NICE capability.
 * @note This method should be called from within the thread context (after
 *       the thread has started).
 *
 * @warning Real-time priorities (HIGH, REALTIME, CRITICAL) can starve other
 *          threads if not used carefully. Only use them for time-critical tasks.
 * @warning This method does NOT throw exceptions on failure; it only prints
 *          a warning to std::cerr.
 *
 * @see PRIORITY - Enum definition for priority levels
 * @see apply_cpu_affinity() - Related thread optimization method
 * @see start() - The method that triggers the priority application
 *
 * @code
 * // Create a critical real-time thread
 * ThreadManager control(
 *     "Control",
 *     0.001,
 *     []() { compute_control(); },
 *     PRIORITY::CRITICAL,  // Will use SCHED_FIFO with priority 99
 *     2                    // Pin to core 2
 * );
 * control.start();  // Priority and affinity are applied in run()
 * @endcode
 */
void ThreadManager::apply_priority() {
#ifdef __linux__
    pthread_t thread_handle = pthread_self();

    struct sched_param param = {0};
    int policy = SCHED_OTHER;
    int nice_value = 0;
    bool use_nice = false;

    // Map to Linux scheduling
    switch (priority_) {
    case PRIORITY::LOWEST:
        policy = SCHED_OTHER;
        param.sched_priority = 0;
        nice_value = 19;
        use_nice = true;
        break;
    case PRIORITY::BACKGROUND:
        policy = SCHED_OTHER;
        param.sched_priority = 0;
        nice_value = 10;
        use_nice = true;
        break;
    case PRIORITY::NORMAL:
        policy = SCHED_OTHER;
        param.sched_priority = 0;
        nice_value = 0;
        use_nice = true;
        break;
    case PRIORITY::HIGH:
        policy = SCHED_RR;
        param.sched_priority = 80;
        break;
    case PRIORITY::REALTIME:
        policy = SCHED_FIFO;
        param.sched_priority = 90;
        break;
    case PRIORITY::CRITICAL:
        policy = SCHED_FIFO;
        param.sched_priority = 99;
        break;
    }

    // pthread_setschedparam() returns the error number directly on failure —
    // it does NOT set errno. strerror() is called on the return value itself.
    // Note: C++17 If-Initializer Syntax
    if (int rc = pthread_setschedparam(thread_handle, policy, &param); rc != 0) {
        std::cerr << "Warning: Failed to set scheduling policy/priority for thread '"
                  << thread_name_ << "': " << std::strerror(rc) << std::endl;
    }

    // setpriority(), unlike pthread_setschedparam(), does follow the errno
    // convention: it returns -1 and sets errno on failure.
    // https://man7.org/linux/man-pages/man3/errno.3.html
    if (use_nice) {
        errno = 0;
        if (setpriority(PRIO_PROCESS, 0, nice_value) != 0) {
            std::cerr << "Warning: Failed to set nice value for thread '"
                      << thread_name_ << "': " << std::strerror(errno) << std::endl;
        }
    }
#endif
}


/**
 * @brief Apply CPU core affinity to the thread.
 *
 * This method pins the thread to a specific CPU core to improve cache locality,
 * reduce context switching, and ensure predictable performance for real-time
 * applications.
 *
 * @details The method uses the Linux pthread_setaffinity_np API to bind the
 *          thread to the CPU core specified by cpu_core_. This is particularly
 *          important for:
 *          - Real-time control loops that need deterministic timing
 *          - High-frequency communication threads (UDP, CAN, etc.)
 *          - Threads that benefit from dedicated cache resources
 *
 * @note This method is only available on Linux systems.
 * @note If cpu_core_ is negative (< 0), affinity is not applied and the
 *       thread can run on any available CPU core.
 * @note This method should be called from within the thread context (after
 *       the thread has started).
 *
 * @warning Setting CPU affinity can degrade performance if the specified core
 *          becomes overloaded. Use with caution in multi-threaded systems.
 * @warning Thread must be joinable and running when this method is called.
 *
 * @see set_cpu_affinity() - Setter for the cpu_core_ member
 * @see apply_priority() - Related thread optimization method
 *
 * @code
 * // Pin control thread to CPU core 2
 * ThreadManager control("Control", 0.001, []() {
 *     compute_control();
 * }, PRIORITY::REALTIME, 2);
 *
 * // Thread will be pinned to core 2 when start() is called
 * control.start();
 * @endcode
 */
void ThreadManager::apply_cpu_affinity() {
#ifdef __linux__
    if (cpu_core_ < 0) return;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core_, &cpuset);

    pthread_t thread_handle = thread_.native_handle();
    if (pthread_setaffinity_np(thread_handle, sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Warning: Failed to set CPU affinity for thread '"
                  << thread_name_ << "' to core " << cpu_core_ << std::endl;
    }
#endif
}


/**
 * @brief Get a const reference to the internal clock instance.
 * @return Const reference to the marinholab::sas::core::Clock object used for timing and statistics.
 * @details Provides access to the clock that manages timing, performance monitoring,
 *          and statistics collection for this thread. This is useful for:
 *          - Reading detailed timing statistics
 *          - Monitoring thread performance in real-time
 *          - Accessing clock configuration and state
 *
 * @note Returns a const reference to prevent modification of the clock's internal state.
 * @note The clock is updated in the thread's main loop via clock_.update_and_sleep().
 *
 * @see get_computation_time()
 * @see get_sleep_time()
 * @see get_effective_sampling_time()
 * @see get_overrun_count()
 * @see get_statistics()
 *
 * @code
 * ThreadManager worker("Worker", 0.01, []() { do_work(); });
 * worker.start();
 *
 * // Access clock information
 * const auto& clock = worker.get_clock();
 * double mean_computation = clock.get_statistics(Statistics::Mean,
 *                                                 marinholab::sas::core::Clock::TimeType::Computational);
 * @endcode
 */
const marinholab::sas::core::Clock& ThreadManager::get_clock() const
{
    return clock_;
}



}
