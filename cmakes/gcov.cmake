
if(DEFINED IS_DEBUG AND IS_DEBUG EQUAL 1 AND NOT DEFINED WIN32)
  # Create the gcov target. Run coverage tests with 'make gcov'
  add_custom_target(gcov
    COMMAND mkdir -p coverage
    COMMAND ${CMAKE_MAKE_PROGRAM} test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  add_custom_command(TARGET gcov
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/ntpc/NTPClient.cpp -o ${OBJECT_DIR}/src/ntpc/
    | grep -A 5 "NTPClient.cpp" > CoverageSummary.tmp
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/ntpc/UDPSocket.cpp -o ${OBJECT_DIR}/src/ntpc/
    | grep -A 5 "UDPSocket.cpp" >> CoverageSummary.tmp
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/main.cpp -o ${OBJECT_DIR}/src/
    | grep -A 5 "main.cpp" >> CoverageSummary.tmp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
  )

  add_custom_command(TARGET gcov
  COMMAND echo "=================== GCOV ===================="
    COMMAND cat CoverageSummary.tmp
    COMMAND echo "-- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
  )

  add_custom_command(TARGET gcov POST_BUILD
    COMMAND lcov --directory ${OBJECT_DIR} -c -o ${CMAKE_BINARY_DIR}/coverage/rapport.info
    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage/rapport.info -o ${CMAKE_BINARY_DIR}/coverage/rapport_filtered.info
    COMMAND genhtml -o . -t "Test cover" rapport_filtered.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
  )
  
  add_dependencies(gcov ${CMAKE_PROJECT_NAME})
  # Make sure to clean up the coverage folder
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)

  # Create the gcov-clean target. This cleans the build as well as generated 
  # .gcda and .gcno files.
  add_custom_target(scrub
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND find ${OBJECT_DIR} -type f -name "*.gcno" | xargs rm 2>/dev/null
    COMMAND find ${OBJECT_DIR} -type f -name "*.gcda" | xargs rm 2>/dev/null
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()