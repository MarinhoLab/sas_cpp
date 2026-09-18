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
 * @file sas_clock_example.cpp
 * @brief Example showing how to use marinholab::sas::core::Clock to time loops.
 */

#include <iostream>

#include <marinholab/sas/core/sas_clock.hpp>

int main(int, char**)
{
    // 10 ms
    marinholab::sas::core::Clock clock(0.01);
    //Alternatively, use marinholab::sas::core::Clock clock(0.01,false); to disable automatic statistics calculation.
    //They do not take much time, but might affect whatever it is that we're trying to time.

    // Always initialize right before the loop, to reduce slowdown in the object allocation/initialization.
    clock.init();
    for(int i=0;i<50;i++)
    {
        // Starting the loop with an update reduces overhead when entering the loop the first time.
        clock.update_and_sleep();
        std::cout << "Loop n = " << i << std::endl;
        std::cout << "  Elapsed time: " << clock.get_elapsed_time_sec() << std::endl;

        std::cout << "  Latest computation time: " << clock.get_time(marinholab::sas::core::Clock::TimeType::Computational) << std::endl;
        std::cout << "  Latest idle time: " << clock.get_time(marinholab::sas::core::Clock::TimeType::Idle) << std::endl;
        std::cout << "  Latest effective thread sampling time: " << clock.get_time(marinholab::sas::core::Clock::TimeType::EffectiveSampling) << std::endl;

        std::cout << "  Desired thread sampling time: " << clock.get_desired_thread_sampling_time_sec() << std::endl;
        std::cout << "  Overrun count: " << clock.get_overrun_count() << std::endl << std::endl;
    }

    //Statistics
    std::cout << "Statistics for the entire loop" << std::endl;
    std::cout << "  Mean computation time: " << clock.get_statistics(marinholab::sas::core::Statistics::Mean,marinholab::sas::core::Clock::TimeType::Computational) << std::endl;
    std::cout << "  Mean idle time: " << clock.get_statistics(marinholab::sas::core::Statistics::Mean,marinholab::sas::core::Clock::TimeType::Idle) << std::endl;
    std::cout << "  Mean effective thread sampling time: " << clock.get_statistics(marinholab::sas::core::Statistics::Mean,marinholab::sas::core::Clock::TimeType::EffectiveSampling) << std::endl;

    return 0;
}
