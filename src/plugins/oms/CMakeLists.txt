set(TARGET_NAME oms)

add_library(${TARGET_NAME}
        common.hpp
        common.cpp
        db.cpp
        db.hpp
        intf.cpp
        intf.hpp
        routing.cpp
        routing.hpp)



target_include_directories(${TARGET_NAME}
        INTERFACE ./)

install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION lib/)