
# help
message(STATUS "Supported distrib.: cmake -DDISTRIBUTION=[debug|release]")
message(STATUS "Supported distrib.: cmake -DCMAKE_BUILD_TYPE=[debug|release]")

# Python for update_sonar_version.py
find_package(Python COMPONENTS Interpreter)

if(NOT DEFINED Python_EXECUTABLE OR Python_EXECUTABLE STREQUAL "")
  message(FATAL_ERROR "Python interpreter not found!")
endif()

# update version
file(READ "${CMAKE_SOURCE_DIR}/version.txt" ver)
string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${ver})
set(VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${ver})
set(VERSION_MINOR ${CMAKE_MATCH_1})

# export options
set(DISTRIBUTION "" CACHE STRING "Distribution type (release or debug)")
set(CMAKE_BUILD_TYPE "" CACHE STRING "Distribution type (release or debug)")

# sanity check
if (NOT DEFINED DISTRIBUTION OR DISTRIBUTION STREQUAL "")
  set(DISTRIBUTION "${CMAKE_BUILD_TYPE}")
  if (DISTRIBUTION STREQUAL "")
    set(DISTRIBUTION "release")
    set(CMAKE_BUILD_TYPE "release")
  endif()
endif()

string(TOLOWER ${DISTRIBUTION} DISTRIBUTION)

set(IS_DEBUG 0)
if (NOT DISTRIBUTION STREQUAL "debug" AND NOT DISTRIBUTION STREQUAL "release")
  message(FATAL_ERROR "Only debug/release supported")
elseif (DISTRIBUTION STREQUAL "debug")
  set(IS_DEBUG 1)
endif()

# Information
message(STATUS "-- Version: ${VERSION_MAJOR}.${VERSION_MINOR}")
message(STATUS "-- Distrib.: ${DISTRIBUTION}")

# update directories
set(NTPC_ROOT_SRC_DIR ${CMAKE_SOURCE_DIR})
set(NTPC_ROOT_BIN_DIR ${CMAKE_BINARY_DIR})
set(NTPC_OUTPUT_BINARY_DIR ${NTPC_ROOT_SRC_DIR}/bin)
set(NTPC_SRC_DIR ${NTPC_ROOT_SRC_DIR}/src)
set(NTPC_INCLUDE_DIR ${NTPC_ROOT_SRC_DIR}/src)

file(MAKE_DIRECTORY ${NTPC_OUTPUT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${NTPC_OUTPUT_BINARY_DIR})
