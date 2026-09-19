cmake_minimum_required(VERSION 3.16)

include(cmake/GetVersion.cmake)
project(marinholab_sas_core VERSION ${SAS_CORE_VERSION} LANGUAGES CXX
        DESCRIPTION "ROS-free C++ core of the SmartArmStack (Clock, ThreadManager, RobotDriver)")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MARINHO_LAB_SAS_CORE_BUILD_EXAMPLES "Build the example executables" ON)

# Shared library by default; build a static library with -DBUILD_SHARED_LIBS=OFF.
# The option() establishes the default while still honouring a value that a
# parent project (or the command line) provides.
option(BUILD_SHARED_LIBS "Build the marinholab_sas_core shared library" ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Eigen3 REQUIRED)

# dqrobotics does not ship a CMake package: locate its header and library manually.
# The public header lives at <prefix>/include/dqrobotics/DQ.h, so we search for
# the path-qualified name. We fail early and with a clear message if it is absent.
find_path(DQROBOTICS_INCLUDE_DIR
    NAMES dqrobotics/DQ.h
    PATHS /usr /usr/local /opt/homebrew
    HINTS ENV CMAKE_PREFIX_PATH)
find_library(DQROBOTICS_LIBRARY
    NAMES dqrobotics
    PATHS /usr /usr/local /opt/homebrew
    HINTS ENV CMAKE_PREFIX_PATH)
if(NOT DQROBOTICS_INCLUDE_DIR OR NOT DQROBOTICS_LIBRARY)
    message(FATAL_ERROR "dqrobotics not found (looked for dqrobotics/DQ.h and libdqrobotics). "
                        "Install it from https://launchpad.net/~dqrobotics-dev/+archive/ubuntu/development "
                        "or build it from https://github.com/dqrobotics/cpp")
endif()

# Model dqrobotics as an imported target so its include dir and library location
# are carried through the build/install regardless of the install prefix.
add_library(dqrobotics::dqrobotics UNKNOWN IMPORTED GLOBAL)
set_target_properties(dqrobotics::dqrobotics PROPERTIES
    IMPORTED_LOCATION "${DQROBOTICS_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${DQROBOTICS_INCLUDE_DIR}")

# ---- Library (shared by default; build static with -DBUILD_SHARED_LIBS=OFF) ----
add_library(marinholab_sas_core
    src/sas_clock.cpp
    src/sas_core.cpp
    src/sas_object.cpp
    src/sas_shutdown_signaler.cpp
    src/sas_robot_driver.cpp
    src/sas_robot_driver_example.cpp
    src/eigen3_std_conversions.cpp
    src/sas_thread_manager.cpp
)
add_library(marinholab::sas::core ALIAS marinholab_sas_core)

set_target_properties(marinholab_sas_core PROPERTIES
    OUTPUT_NAME marinholab_sas_core
    # Exported as marinholab::sas::core. (No install(EXPORT NAMESPACE): CMake
    # concatenates the namespace prefix with EXPORT_NAME without a separator,
    # which would mangle a nested namespace like marinholab::sas::core.)
    EXPORT_NAME "marinholab::sas::core"
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

target_include_directories(marinholab_sas_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(marinholab_sas_core PUBLIC Eigen3::Eigen dqrobotics::dqrobotics)

# ---- Examples ----
if(MARINHO_LAB_SAS_CORE_BUILD_EXAMPLES)
    foreach(_example IN ITEMS sas_core_example sas_clock_example
                             sas_clock_sched_fifo_example thread_manager_example)
        add_executable(${_example} examples/${_example}.cpp)
        target_link_libraries(${_example} PRIVATE marinholab_sas_core)
    endforeach()
    add_executable(sas_robot_driver_example examples/sas_robot_driver_example_main.cpp)
    target_link_libraries(sas_robot_driver_example PRIVATE marinholab_sas_core)
endif()

# ---- Install ----
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS marinholab_sas_core
        EXPORT marinholab_sas_core-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(MARINHO_LAB_SAS_CORE_BUILD_EXAMPLES)
    install(TARGETS sas_core_example sas_clock_example sas_clock_sched_fifo_example
                    thread_manager_example sas_robot_driver_example
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# ---- CMake package ----
include(CMakePackageConfigHelpers)
install(EXPORT marinholab_sas_core-targets
        FILE marinholab_sas_core-targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/marinholab_sas_core)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/marinholab_sas_core-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/marinholab_sas_core-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/marinholab_sas_core)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/marinholab_sas_core-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/marinholab_sas_core-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/marinholab_sas_core-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/marinholab_sas_core)
