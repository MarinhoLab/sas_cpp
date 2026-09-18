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
 * @file sas_clock_sched_fifo_example.cpp
 * @brief Example demonstrating Clock with realtime SCHED_FIFO.
 */

#include <iostream>

#include <marinholab/sas/core/sas_clock.hpp>

int main(int, char**)
{
    // 1 ms
    marinholab::sas::core::Clock clock(0.001);
    //Alternatively, use marinholab::sas::core::Clock clock(0.01,false); to disable automatic statistics calculation.
    //They do not take much time, but might affect whatever it is that we're trying to time.

    //Set the communication thread to be realtime with SCHED_FIFO.
    sched_param sch;
    int policy;
    pthread_getschedparam(pthread_self(), &policy, &sch);
    sch.sched_priority = 20;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch)) {
        std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
    }

    // Always initialize right before the loop, to reduce slowdown in the object allocation/initialization.
    clock.init();
    for(int i=0;i<50;i++)
    {
        // Starting the loop with an update reduces overhead when entering the loop the first time.
        clock.update_and_sleep();
    }

    //Statistics
    std::cout << "Statistics for the entire loop" << std::endl;
    std::cout << "  Mean computation time: " << clock.get_statistics(marinholab::sas::core::Statistics::Mean,marinholab::sas::core::Clock::TimeType::Computational) << std::endl;
    std::cout << "  Mean idle time: " << clock.get_statistics(marinholab::sas::core::Statistics::Mean,marinholab::sas::core::Clock::TimeType::Idle) << std::endl;
    std::cout << "  Mean effective thread sampling time: " << clock.get_statistics(marinholab::sas::core::Statistics::Mean,marinholab::sas::core::Clock::TimeType::EffectiveSampling) << std::endl;
    std::cout << "  Overrun count: " << clock.get_overrun_count() << std::endl << std::endl;

    return 0;
}
