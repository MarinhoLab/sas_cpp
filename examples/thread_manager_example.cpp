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
#   Author: Juan Jose Quiroz Omana (juanjose.quirozomana@manchester.ac.uk)
#
# ################################################################*/

#include <marinholab/sas/core/sas_thread_manager.hpp>
#include <iostream>
#include <thread>

int main() {
    int counter = 0;

    // Create and start thread
    marinholab::sas::core::ThreadManager tm("test", 0.1, [&counter]() {
        std::cout << ++counter << "\n";
    });

    tm.start();

    // Run for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Clean up
    tm.stop();
    return 0;
}
