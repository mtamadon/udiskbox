CMAKE_MINIMUM_REQUIRED(VERSION 3.4)
PROJECT(udiskbox)
ENABLE_LANGUAGE(C ASM)

set(CMAKE_TOOLCHAIN_FILE /home/skt/code/stm32-cmake/cmake/gcc_stm32.cmake)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH /home/skt/code/stm32-cmake/cmake)
set(STM32Cube_DIR /home/skt/code/STM32Cube_FW_F1_V1.2.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=gbk")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=gbk")

add_subdirectory(src)
