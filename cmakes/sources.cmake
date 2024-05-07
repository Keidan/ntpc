

configure_file (
  "${NTPC_SRC_DIR}/config.h.in"
  "${NTPC_SRC_DIR}/config.h"
)
add_custom_target(
 update_sonar_version ALL
 COMMAND ${Python_EXECUTABLE} ${NTPC_ROOT_SRC_DIR}/update_sonar_version.py --root ${NTPC_ROOT_SRC_DIR}
 COMMENT "Target update_sonar_version"
)


set(ntpc_src ${NTPC_SRC_DIR}/ntpc/NTPClient.cpp)
set(socket_src ${NTPC_SRC_DIR}/ntpc/UDPSocket.cpp)
set(main_src ${NTPC_SRC_DIR}/main.cpp)
set(common_src ${ntpc_src} ${socket_src})
set(bin_src ${common_src} ${main_src})
if(WIN32)
  set(bin_src ${bin_src} ${NTPC_SRC_DIR}/win32/getopt.c ${NTPC_SRC_DIR}/win32/getopt_long.c ${NTPC_SRC_DIR}/win32/getopt.h)
endif()

add_executable(${NTPC_PROJECT_NAME} ${bin_src})


if(MSVC)
  set_target_properties(${NTPC_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${NTPC_OUTPUT_BINARY_DIR})
  set_target_properties(${NTPC_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${NTPC_OUTPUT_BINARY_DIR})
endif(MSVC)

add_dependencies(${NTPC_PROJECT_NAME} update_sonar_version)

