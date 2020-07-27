set(TARGET_NAME utils)

add_library(${TARGET_NAME}
        intfutils.cpp
        intfutils.hpp
        utils.cpp
        utils.hpp)

target_include_directories(${TARGET_NAME}
        INTERFACE ./)

install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION lib/)