list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")

# check for required packages
find_package(VPP) #use FindVPP.cmake


set(UNIT_TEST_LIST
        intf)

foreach(NAME IN LISTS UNIT_TEST_LIST)
    list(APPEND UNIT_TEST_SOURCE_LIST
            ${NAME}.test.cpp)
endforeach()


foreach(NAME IN LISTS OMS_DIR)
    list(APPEND OMS_SOURCE_LIST ${NAME}.cpp)
endforeach()


set(TARGET_NAME tests)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=gnu++11 ${RIGOROUS_CXX_FLAGS} -lvapiclient -lpthread -lboost_system")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -std=gnu++11 -g -O0 ${RIGOROUS_CXX_FLAGS} -lvapiclient -lpthread -lboost_system")
#NDEBUG to skip assert checks
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -std=gnu++11 -DNDEBUG -O2 ${RIGOROUS_CXX_FLAGS} -lvapiclient -lpthread -lboost_system")

add_executable(${TARGET_NAME}
        main.cpp
        intf.test.cpp
        ../oms/common.cpp
        ../oms/db.cpp
        ../oms/intf.cpp
        ../oms/routing.cpp
        ../utils/utils.cpp
        ../utils/intfutils.cpp
        ${UNIT_TEST_SOURCE_LIST})

#target_link_libraries(${TARGET_NAME}
#        PUBLIC utils)
target_link_libraries(
        ${TARGET_NAME} ${VOM_LIBRARY}
)
target_include_directories(${TARGET_NAME}
        PUBLIC ../externals/catch2/)

add_test(
        NAME ${TARGET_NAME}
        COMMAND ${TARGET_NAME} -o report.xml -r junit)