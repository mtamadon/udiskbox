FIND_PACKAGE(CMSIS REQUIRED)
FIND_PACKAGE(STM32HAL REQUIRED)

SET(STORAGE_DRIVER SDCARD)
FIND_PACKAGE(FATFS REQUIRED)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
    ${FATFS_INCLUDE_DIRS}
)

aux_source_directory(. MAIN_SRCS)
include_directories(./DS1302)
aux_source_directory(./DS1302 DS1302_SRCS)
include_directories(./RFID)
aux_source_directory(./RFID RFID_SRCS)
include_directories(./Usart)
aux_source_directory(./Usart USART_SRCS)

list(REMOVE_ITEM MAIN_SRCS
    ./web_server.c
    ./ip_arp_udp_tcp.c
)

SET(PROJECT_SOURCES
    ${MAIN_SRCS}
    ${DS1302_SRCS}
    ${RFID_SRCS}
    ${USART_SRCS}
)


SET(STM32_LINKER_SCRIPT ${CMSIS_LINKER_SCRIPT})

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${CMSIS_SOURCES}
    ${STM32HAL_SOURCES}
    ${FATFS_SOURCES}
)

STM32_SET_TARGET_PROPERTIES(${CMAKE_PROJECT_NAME})
STM32_ADD_HEX_BIN_TARGETS(${CMAKE_PROJECT_NAME})

IF(EXECUTABLE_OUTPUT_PATH)
    SET(FLASH_SCRIPT_FILENAME "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}_flash.tcl")
    SET(BIN_FILENAME "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.bin")
    SET(ELF_FILENAME "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}")
ELSE()
    SET(FLASH_SCRIPT_FILENAME "${CMAKE_PROJECT_NAME}_flash.tcl")
    SET(BIN_FILENAME "${CMAKE_PROJECT_NAME}.bin")
    SET(ELF_FILENAME "${CMAKE_PROJECT_NAME}")
ENDIF()

#SET(PROGRAMMER jlink)
SET(PROGRAMMER stlink-v2)

SET(FLASH_SCRIPT 
    "set proDir [pwd]\n"
    "\n"
    "source [find interface/${PROGRAMMER}.cfg]\n"
    "source [find target/stm32f1x.cfg]\n"
    "\n"
    "init\n"
    "#halt\n"
    "\n"
    "reset halt\n"
    "\n"
    "flash probe 0\n"
    "stm32f1x mass_erase 0\n"
    "flash write_bank 0 $proDir/${BIN_FILENAME} 0\n"
    "reset run\n"
    "\n"
    "shutdown\n"
)
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${FLASH_SCRIPT_FILENAME} ${FLASH_SCRIPT})

SET(GDB_CONF
    "define prog\n"
    "monitor reset halt\n"
    "monitor flash probe 0\n"
    "monitor stm32f1x mass_erase 0\n"
    "monitor flash write_bank 0 ${BIN_FILENAME} 0\n"
    "monitor reset halt\n"
    "end\n"
    "\n"
    "define reset\n"
    "monitor reset halt\n"
    "end\n"
    )
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/.gdbinit ${GDB_CONF})

ADD_CUSTOM_TARGET(PROGRAMCHIP
    DEPENDS ${BIN_FILENAME}
    COMMAND openocd -f ${FLASH_SCRIPT_FILENAME}
)

SET(DEBUG_COMMAND arm-none-eabi-gdb
    ${ELF_FILENAME}
    --eval-command='target remote|openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c \"gdb_port pipe\" -c \"log_output openocd.log\"'
    -x .gdbinit
)
message(STATUS ${DEBUG_COMMAND})
ADD_CUSTOM_TARGET(DEBUGONCHIP
    DEPENDS ${BIN_FILENAME}
    COMMAND ${DEBUG_COMMAND}
)

