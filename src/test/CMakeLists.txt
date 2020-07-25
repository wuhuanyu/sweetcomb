set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

add_executable (oms_test oms_test.cpp)
target_link_libraries (oms_test ${Boost_LIBRARIES})
target_link_libraries (oms_test "../oms/")