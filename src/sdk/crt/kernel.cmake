# SPDX-FileCopyrightText: (C) 2023 yuuma03
# SPDX-License-Identifier: BSD-3-Clause

set(KERNEL_SOURCES
    ${SOURCES}
    string/strdup.c
    string/strndup.c)

add_library(kcrt "knostdlib" STATIC ${KERNEL_SOURCES})

target_include_directories(kcrt PUBLIC include)
target_compile_options(kcrt PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:-ffreestanding>)
