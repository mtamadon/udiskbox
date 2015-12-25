cmake --debug-output \
    -DSTM32_CHIP=STM32F103ZE \
    -DCMAKE_TOOLCHAIN_FILE=/home/skt/code/stm32-cmake/cmake/gcc_stm32.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_MODULE_PATH=/home/skt/code/stm32-cmake/cmake \
    -DSTM32Cube_DIR=/home/skt/code/STM32Cube_FW_F1_V1.2.0 \
    /home/skt/code/udiskbox/
