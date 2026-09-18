#pragma once
/*
# Copyright (c) 2022-2023 Murilo Marques Marinho
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
 * @file sas_object.hpp
 * @brief Base object class for identification and license utilities.
 *
 * Defines the marinholab::sas::core::Object class standardises sas Objects, such as license header printing.
 */
#include <string>

namespace marinholab::sas::core
{
/**
 * @brief Base class for SAS objects.
 */
class Object
{
protected:
    const std::string class_name_;
    void _print_license_header(const std::string& class_name);
    Object(const std::string& class_name);
public:
    Object() = delete;
    /**
     * @brief Get the class name identifier
     * @return The class name as a std::string
     */
    std::string get_class_name() const;
};
}
