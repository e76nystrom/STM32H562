#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 4.2)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(STM32H562 C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Uncomment for hardware floating point
#add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
#add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
#add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

add_compile_options(-mcpu=cortex-m33 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

add_compile_options(-Og -g)

include_directories(Core/Inc Drivers/STM32H5xx_HAL_Driver/Inc Drivers/STM32H5xx_HAL_Driver/Inc/Legacy Drivers/CMSIS/Device/ST/STM32H5xx/Include Drivers/CMSIS/Include)

add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32H562xx)

set(WIZNET_BASE_DIR ${CMAKE_SOURCE_DIR}/../ioLibrary_Driver)
set(U8X8_BASE_DIR ${CMAKE_SOURCE_DIR}/../)

file(GLOB_RECURSE SOURCES "Core/*.*" "Drivers/*.*")

set(WIZNET_SOURCE
    ${WIZNET_BASE_DIR}/Ethernet/wizchip_conf.c
    ${WIZNET_BASE_DIR}/Ethernet/socket.c
    ${WIZNET_BASE_DIR}/Ethernet/W5500/w5500.c
    ${WIZNET_BASE_DIR}/Internet/DNS/dns.c
    ${CMAKE_SOURCE_DIR}/src/wiznet.c
)
#	${WIZNET_BASE_DIR}/Internet/DHCP/dhcp.c

set(RADIOLIB_SOURCE
    src/main.cpp
    src/dhcp.c
    ${CMAKE_SOURCE_DIR}/../gpsLib/src/gpsLib.cpp
    ${CMAKE_SOURCE_DIR}/../oledLib/src/oledLib.cpp
)
#    src/serialio.cpp

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32H562RGTX_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m33 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

set(TARGET_NAME ${PROJECT_NAME}.elf)

add_executable(${TARGET_NAME}
    ${SOURCES}
    ${WIZNET_SOURCE}
    ${RADIOLIB_SOURCE}
    ${LINKER_SCRIPT}
)

add_compile_definitions(
    STM32_BUILD
    MULTI_CORE
    UART_ATOMIC
    TCP_SERVER
    STM32H5
    _WIZCHIP_=5500
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${WIZNET_BASE_DIR}/Ethernet
    ${WIZNET_BASE_DIR}/Ethernet/W5500
    ${WIZNET_BASE_DIR}/Internet/DHCP
    ${WIZNET_BASE_DIR}/Internet/DNS
    ../gpsLib/src
    ../oledLib/src
    ../RadioLib/src
)

file(GLOB U8G2_SOURCES ../u8g2/*.c)
add_library(u8g2 STATIC ${U8G2_SOURCES})

target_include_directories(u8g2 PUBLIC ../u8g2)

set(RADIOLIB_DIR "${CMAKE_SOURCE_DIR}/../RadioLib" CACHE PATH "Path to RadioLib repo")
set(RL_SRC ${RADIOLIB_DIR}/src)

add_library(radioLib STATIC
    ${RL_SRC}/Module.cpp
    ${RL_SRC}/Hal.cpp
    ${RL_SRC}/protocols/PhysicalLayer/PhysicalLayer.cpp
    ${RL_SRC}/modules/SX126x/SX126x_commands.cpp
    ${RL_SRC}/modules/SX126x/SX126x_LR_FHSS.cpp
    ${RL_SRC}/modules/SX126x/SX126x_config.cpp
    ${RL_SRC}/modules/SX126x/SX126x.cpp
    ${RL_SRC}/modules/SX126x/SX1262.cpp
    ${RL_SRC}/modules/nRF24/nRF24.cpp
    ${RL_SRC}/utils/ConvCode.cpp
    ${RL_SRC}/utils/Utils.cpp
    ${RL_SRC}/utils/CRC.cpp
    ${RL_SRC}/utils/Cryptography.cpp
    src/radioLibHal.cpp
    src/radioLib.cpp
)
#    ${RL_SRC}/utils/FEC.cpp

target_include_directories(radioLib PRIVATE
    ${CMAKE_SOURCE_DIR}/include				    
    ${CMAKE_SOURCE_DIR}/../RadioLib/src
)

target_compile_definitions(radioLib PRIVATE
    RADIOLIB_STATIC_ONLY=1
    RADIOLIB_EXCLUDE_LORAWAN
    RADIOLIB_EXCLUDE_APRS
    RADIOLIB_EXCLUDE_AX25
    RADIOLIB_EXCLUDE_PAGER
    RADIOLIB_EXCLUDE_RTTY
    RADIOLIB_EXCLUDE_MORSE
    RADIOLIB_EXCLUDE_HELLSCHREIBER
    RADIOLIB_EXCLUDE_SSTV
    RADIOLIB_EXCLUDE_FSK4
    RADIOLIB_EXCLUDE_BELL
)
    #RADIOLIB_EXCLUDE_DIRECT_RECEIVE

target_link_libraries(${TARGET_NAME}
        u8g2
	radioLib
)
#        ${CMAKE_SOURCE_DIR}/../gpsLib/src/gpsLib.cpp
#

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")


