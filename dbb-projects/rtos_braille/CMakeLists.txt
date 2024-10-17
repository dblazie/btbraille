set(USERHOME $ENV{HOME})
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico CACHE STRING "Board type")
# Pull in Raspberry Pi Pico SDK (must be before project)
set(PICO_BOARD pico)

include(pico_sdk_import.cmake)  
#include_directories(AFTER include)
#include_directories(${PWD})
# Pull in FreeRTOS
include(FreeRTOS_Kernel_import.cmake)
# FreeRTOS configurations
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_SOURCE_DIR}" CACHE STRING "")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
set(USERHOME $ENV{HOME})
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
#set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================

project(rtos_braille C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()
if (NOT TARGET _FreeRTOS_kernel_inclusion_marker)
    add_library(_FreeRTOS_kernel_inclusion_marker INTERFACE)

    # Pull in PICO SDK (must be before project)
    include(pico_sdk_import.cmake)
    if (PICO_SDK_VERSION_STRING VERSION_LESS "1.2.0")
        message(FATAL_ERROR "Require at least Raspberry Pi Pico SDK version 1.2.0")
    endif()

    if (NOT FREERTOS_KERNEL_PATH)
        get_filename_component(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../.. REALPATH)
    endif ()

    message(DEBUG "FREERTOS_KERNEL_PATH is ${FREERTOS_KERNEL_PATH}")
    project(FreeRTOS-Kernel C CXX)

    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)

    pico_is_top_level_project(FREERTOS_KERNEL_TOP_LEVEL_PROJECT)

    # The real work gets done in library.cmake which is called at the end of pico_sdk_init
    list(APPEND PICO_SDK_POST_LIST_FILES ${CMAKE_CURRENT_LIST_DIR}/library.cmake)

    # We need to inject the following header file into ALL SDK files (which we do via the config header)
    list(APPEND PICO_CONFIG_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/include/freertos_sdk_config.h)

    if (FREERTOS_KERNEL_TOP_LEVEL_PROJECT)
        message("FreeRTOS: initialize SDK since we're the top-level")
        # Initialize the SDK
        pico_sdk_init()
    else()
        set(PICO_SDK_POST_LIST_FILES ${PICO_SDK_POST_LIST_FILES} PARENT_SCOPE)
        set(PICO_CONFIG_HEADER_FILES ${PICO_CONFIG_HEADER_FILES} PARENT_SCOPE)
        set(FREERTOS_KERNEL_PATH ${FREERTOS_KERNEL_PATH} PARENT_SCOPE)
    endif()
endif()


# Add executable. Default name is the project name, version 0.1
add_executable(rtos_braille
	braille.c
	spi_braille.c
	FreeRTOSConfig.h
	rtos_braille.c
)

target_include_directories(rtos_braille PRIVATE ${CMAKE_CURRENT_LIST_DIR}/)
target_include_directories(rtos_braille PRIVATE /home/deane/freeRtos/pico-sdk/src/rp2_common/pico_async_context/include/)
# pull in common dependencies
target_link_libraries(rtos_braille pico_stdlib hardware_spi FreeRTOS-Kernel 
    FreeRTOS-Kernel-Heap4
    pico_async_context_freertos
    FreeRTOS-Kernel-Heap4
    pico_stdlib
    )

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(rtos_braille)

# call pico_set_program_url to set path to example on github, so users can find the source for an example via picotool
#target_include_directories(dbb-projects PRIVATE ${CMAKE_SOURCE_DIR})
