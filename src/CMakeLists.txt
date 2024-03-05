
# This file is a part of MRNIU/opensbi_interface
# (https://github.com/MRNIU/opensbi_interface).
#
# CMakeLists.txt for MRNIU/opensbi_interface.

add_library(opensbi_interface STATIC
        include/opensbi_interface.h
        opensbi_interface.c
)

target_compile_options(opensbi_interface PRIVATE
        # 如果 CMAKE_BUILD_TYPE 为 Release 则使用 -O3 -Werror，否则使用 -O0 -g -ggdb
        $<$<CONFIG:Release>:-O3;-Werror>
        $<$<CONFIG:Debug>:-O0;-g;-ggdb>
        # 打开全部警告
        -Wall
        # 打开额外警告
        -Wextra
        # 启用异常处理机制
        -fexceptions
        # 启用 free-standing 环境
        -ffreestanding
)

target_link_options(opensbi_interface PRIVATE
        # 不链接 ctr0 等启动代码
        -nostartfiles
)
