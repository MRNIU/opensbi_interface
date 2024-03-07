
# This file is a part of MRNIU/opensbi_interface
# (https://github.com/MRNIU/opensbi_interface).
#
# 3rd.cmake for MRNIU/opensbi_interface.
# 依赖管理

# 设置依赖下载路径
set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/3rd)
# 优先使用本地文件
set(CPM_USE_LOCAL_PACKAGES True)
# https://github.com/cpm-cmake/CPM.cmake
# -------- get_cpm.cmake --------
set(CPM_DOWNLOAD_VERSION 0.38.2)

if (CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif (DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else ()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif ()

# Expand relative path. This is important if the provided path contains a tilde (~)
get_filename_component(CPM_DOWNLOAD_LOCATION ${CPM_DOWNLOAD_LOCATION} ABSOLUTE)

function(download_cpm)
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
            https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
            ${CPM_DOWNLOAD_LOCATION}
    )
endfunction()

if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    download_cpm()
else ()
    # resume download if it previously failed
    file(READ ${CPM_DOWNLOAD_LOCATION} check)
    if ("${check}" STREQUAL "")
        download_cpm()
    endif ()
    unset(check)
endif ()

include(${CPM_DOWNLOAD_LOCATION})
# -------- get_cpm.cmake --------

# https://github.com/riscv-software-src/opensbi
CPMAddPackage(
        NAME opensbi
        GIT_REPOSITORY https://github.com/riscv-software-src/opensbi.git
        GIT_TAG v1.4
        VERSION 1.4
        DOWNLOAD_ONLY True
)
if (opensbi_ADDED)
    # 编译 opensbi
    add_custom_target(opensbi
            COMMENT "build opensbi..."
            # make 时编译
            ALL
            WORKING_DIRECTORY ${opensbi_SOURCE_DIR}
            COMMAND
            ${CMAKE_COMMAND}
            -E
            make_directory
            ${opensbi_BINARY_DIR}
            COMMAND
            make
            CROSS_COMPILE=${TOOLCHAIN_PREFIX}
            FW_JUMP=y
            FW_JUMP_ADDR=0x80200000
            PLATFORM_RISCV_XLEN=64
            PLATFORM=generic
            O=${opensbi_BINARY_DIR}
            COMMAND
            ${CMAKE_COMMAND}
            -E
            copy_directory
            ${opensbi_SOURCE_DIR}/include
            ${opensbi_BINARY_DIR}/include
    )
endif ()

# https://github.com/cpm-cmake/CPMLicenses.cmake
# 保持在 CPMAddPackage 的最后
CPMAddPackage(
        NAME CPMLicenses.cmake
        GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
        VERSION 0.0.7
)
if (CPMLicenses.cmake_ADDED)
    cpm_licenses_create_disclaimer_target(
            write-licenses "${CMAKE_CURRENT_SOURCE_DIR}/3rd/LICENSE" "${CPM_PACKAGES}"
    )
endif ()
# make 时自动在 3rd 文件夹下生成 LICENSE 文件
add_custom_target(3rd_licenses
        ALL
        COMMAND
        make
        write-licenses
)

# doxygen
find_package(Doxygen
        REQUIRED dot)
if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen not found.\n"
            "Following https://www.doxygen.nl/index.html to install.")
endif ()
