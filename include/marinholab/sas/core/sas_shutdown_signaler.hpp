#pragma once
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
#   Author: Murilo M. Marinho, email: murilomarinho@ieee.org
#
# ################################################################*/

/**
 * @file sas_shutdown_signaler.hpp
 * @brief Cross-module shutdown signaling class.
 *
 * Provides a class that coordinates shutdown requests between different
 * parts of the system. Useful to unify shutdown behavior in both ROS and
 * non-ROS contexts.
 */
#include<atomic>

namespace marinholab::sas::core
{
    /**
     * @brief Shutdown coordinator.
     *
     * ShutdownSignaler provides a thread-safe mechanism to unify shutdown
     * requests.
     */
    class ShutdownSignaler
    {
        private:
            std::atomic_bool* external_shutdown_signal_{nullptr};
            bool internal_shutdown_signal_{false};
        public:
            /**
             * @brief Default constructor
             */
            ShutdownSignaler() = default;
            /**
             * @brief Construct a ShutdownSignaler using an external atomic flag
             * @param external_shutdown_signal Pointer to an external atomic_bool used for shutdown signaling
             */
            ShutdownSignaler(std::atomic_bool* external_shutdown_signal):
            external_shutdown_signal_(external_shutdown_signal)
            {

            };

            /**
             * @brief Check whether a shutdown has been requested
             * @return true if shutdown requested (external or internal), false otherwise
             */
            bool should_shutdown();

            /**
             * @brief Trigger a shutdown signal
             */
            void shutdown();
    };
}
