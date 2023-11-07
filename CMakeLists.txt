cmake_minimum_required(VERSION 3.20)

project(ctr)

add_library(ctr_headers INTERFACE)
target_include_directories(ctr_headers INTERFACE include)

set_target_properties(ctr_headers PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED Yes
  CXX_EXTENSIONS No)

add_subdirectory(src)

find_package(Doxygen)

if (DOXYGEN_FOUND)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(Doxyfile.in ${DOXYGEN_OUT} @ONLY)
  message("Adding docs target")

  add_custom_target(docs ALL
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating docs"
                    VERBATIM)
else()
  message("Could not find doxygen - docs target is not available")
endif()

configure_file(test/util/test.config.py.in ${CMAKE_CURRENT_BINARY_DIR}/test.py @ONLY)
