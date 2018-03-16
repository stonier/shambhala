##############################################################################
# Cmake
##############################################################################

cmake_minimum_required(VERSION 3.5)
project(drake_systems)

##############################################################################
# Find Packages
##############################################################################

find_package(ament_cmake REQUIRED)
#find_package(ecl_build CONFIG REQUIRED)

# make sure to preset the drake_DIR, e.g. in the initial cache file
#   set(drake_DIR /opt/drake/lib/cmake/drake CACHE PATH "")
find_package(drake CONFIG REQUIRED)
# If we use the abstract layer instead
# find_package(drake_ros REQUIRED)

##############################################################################
# Project Configuration
##############################################################################

#ecl_enable_cxx14_compiler()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wl,--no-as-needed")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# This one causes problems with YAML errors: -Wl,--no-as-needed?

##############################################################################
# Sources
##############################################################################

#add_subdirectory(include)
add_subdirectory(src)

##############################################################################
# Exports
##############################################################################

# ament_export_interfaces(${PROJECT_NAME})
# ament_export_dependencies(ecl_config)
ament_package()
