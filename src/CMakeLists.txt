
# This file is a part of MRNIU/opensbi_interface
# (https://github.com/MRNIU/opensbi_interface).
#
# CMakeLists.txt for MRNIU/opensbi_interface.

add_library(${PROJECT_NAME} STATIC
        opensbi_interface.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(opensbi_interface PRIVATE
        $<$<CONFIG:Release>:-O3;-Werror>
        -Wall
        -Wextra
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "include/opensbi_interface.h")

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        PUBLIC_HEADER DESTINATION include
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
install(EXPORT ${PROJECT_NAME}-targets
        NAMESPACE ${PROJECT_NAME}::
        FILE ${PROJECT_NAME}-config.cmake
        DESTINATION lib/cmake/${PROJECT_NAME})
