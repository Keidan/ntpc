cmake_minimum_required(VERSION 3.22)

set(NTPC_PROJECT_NAME ntpc)
project(${NTPC_PROJECT_NAME} C CXX)

set(CMAKES_DIR ${PROJECT_SOURCE_DIR}/cmakes)
set(cmakes  ${CMAKES_DIR}/envs.cmake
            ${CMAKES_DIR}/options.cmake
            ${CMAKES_DIR}/sources.cmake
            ${CMAKES_DIR}/gcov.cmake
)

foreach(file ${cmakes})
  include(${file})
endforeach()

