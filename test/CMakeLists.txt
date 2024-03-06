
# This file is a part of MRNIU/opensbi_interface
# (https://github.com/MRNIU/opensbi_interface).
#
# CMakeLists.txt for MRNIU/opensbi_interface.

# 设置项目名与版本
project(
        test
)

enable_language(ASM C)

add_executable(${PROJECT_NAME}
        main.c
        boot.S
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ../src/include
)

target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -ffreestanding
)

target_link_options(${PROJECT_NAME} PRIVATE
        -nostartfiles
        -no-pie
        -T ${PROJECT_SOURCE_DIR}/link.ld
)

# 添加要链接的库
target_link_libraries(${PROJECT_NAME} PRIVATE
        opensbi_interface
)

add_dependencies(${PROJECT_NAME}
        opensbi
        opensbi_interface
)

add_custom_target(${PROJECT_NAME}_run DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMAND
        qemu-system-riscv64
        -serial stdio
        -monitor telnet::2333,server,nowait
        -machine virt
        -nographic
        -bios ${opensbi_BINARY_DIR}/platform/generic/firmware/fw_jump.elf
        -kernel $<TARGET_FILE:${PROJECT_NAME}>
)
