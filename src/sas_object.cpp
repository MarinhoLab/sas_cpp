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
 * @file sas_object.cpp
 * @brief Implementation of the Object base class.
 */
#include "marinholab/sas/core/sas_object.hpp"

#include <iostream>

namespace marinholab::sas::core
{

/**
 * @brief Object::_print_license_header A Copyright header to help enforce LGPLv3 compliance.
 *
 * If you modify this method in any shape or form, you have therefore made a derivative work of
 * this library, hence you must abide to the Copyleft side of the LGPLv3.
 */
void Object::_print_license_header(const std::string& class_name)
{
    // Using colors: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
    std::string blue_text("\033[1;34m");
    std::string default_color("\033[0m");
    
    
    std::cout << blue_text << std::string(class_name.size(),'*') 
        << "****************************************************************" << default_color << std::endl;
    std::cout << blue_text << class_name 
        + " (c) Murilo M. Marinho (murilomarinho.info) 2016-2026 LGPLv3" << default_color << std::endl;
    std::cout << blue_text << std::string(class_name.size(),'*') 
        << "****************************************************************" << default_color << std::endl;
}

Object::Object(const std::string &class_name):
    class_name_(class_name)
{
    _print_license_header(class_name);
}

std::string marinholab::sas::core::Object::get_class_name() const
{
    return class_name_;
}


}
