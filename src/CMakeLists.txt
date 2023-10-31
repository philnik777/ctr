
add_library(ctr dummy.cpp)

target_link_libraries(ctr ctr_headers)

set_target_properties(ctr PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED Yes
  CXX_XTENSIONS No)
