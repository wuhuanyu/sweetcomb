set(UNIT_TEST_LIST
        oms
        db)

foreach(NAME IN LISTS UNIT_TEST_LIST)
    list(APPEND UNIT_TEST_SOURCE_LIST
            ${NAME}.test.cpp)
endforeach()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=gnu++11 ${RIGOROUS_CXX_FLAGS} -lvapiclient -lboost_system -lpthread")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -std=gnu++11 -g -O0 ${RIGOROUS_CXX_FLAGS} -lvapiclient -lboost_system -lpthread")
#NDEBUG to skip assert checks
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -std=gnu++11 -DNDEBUG -O2 ${RIGOROUS_CXX_FLAGS} -lvapiclient -lpthread -lboost_system")

set(TARGET_NAME tests)

add_executable(${TARGET_NAME}
        main.cpp
        ${UNIT_TEST_SOURCE_LIST})

target_link_libraries(${TARGET_NAME}
        PUBLIC oms utils ${VOM_LIBRARY} )

target_include_directories(${TARGET_NAME}
        PUBLIC ../externals/catch2/)

add_test(
        NAME ${TARGET_NAME}
        COMMAND ${TARGET_NAME} -o report.xml -r junit)