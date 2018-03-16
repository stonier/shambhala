##############################################################################
# Cmake
##############################################################################

cmake_minimum_required(VERSION 3.5)
project(drake_systems)

##############################################################################
# Find Packages
##############################################################################

set(drake_DIR /opt/drake/lib/cmake/drake CACHE PATH "")
find_package(drake CONFIG REQUIRED)

##############################################################################
# Project Configuration
##############################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wl,--no-as-needed")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# This one causes problems with YAML errors: -Wl,--no-as-needed?

##############################################################################
# Sources
##############################################################################

add_subdirectory(src)
