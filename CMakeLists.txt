cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Cross-compilation settings
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME fightg474)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

set(CMSIS_DSP_PATH "D:/keli arm/ARM/CMSIS/5.8.0/CMSIS/DSP" CACHE PATH "Path to CMSIS-DSP library")

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyHDL/My_SPI.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyFML/ad9833.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyFML/adc_fml.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyFML/fft_fml.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/st7735s/st7735s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyBLL/adc_bll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyBLL/dds_bll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyBLL/fft_bll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyAPL/adc_apl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyAPL/dds_apl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyAPL/fft_apl.c
    ${CMSIS_DSP_PATH}/Source/CommonTables/arm_common_tables.c
    ${CMSIS_DSP_PATH}/Source/CommonTables/arm_const_structs.c
    ${CMSIS_DSP_PATH}/Source/TransformFunctions/arm_bitreversal2.c
    ${CMSIS_DSP_PATH}/Source/TransformFunctions/arm_cfft_f32.c
    ${CMSIS_DSP_PATH}/Source/TransformFunctions/arm_cfft_radix8_f32.c
    ${CMSIS_DSP_PATH}/Source/ComplexMathFunctions/arm_cmplx_mag_f32.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyHDL
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyFML
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyBLL
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/MyAPL
    ${CMAKE_CURRENT_SOURCE_DIR}/mystuff/st7735s
    ${CMSIS_DSP_PATH}/Include
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
