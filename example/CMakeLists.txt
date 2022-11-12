project(ir_print)

cmake_minimum_required( VERSION 3.5 )

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set (PREFIX "avr-")

set(CMAKE_ASM_COMPILER ${PREFIX}gcc)
set(CMAKE_C_COMPILER ${PREFIX}gcc)
set(CMAKE_AR ${PREFIX}ar)

set(CPU_FREQ_MHZ 16000000)
set(DEVICE "atmega2560")
set(TTY_DEVICE "/dev/ttyUSB0")
set(TTY_BAUDRATE 115200)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmcu=${DEVICE} -Wall -DF_CPU=${CPU_FREQ_MHZ}UL")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

set(GENERATED_BINARY "ir_print")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmcu=${DEVICE}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Wl,-Map,${GENERATED_BINARY}.map")

set(IR_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/../")

link_directories(
    ${PROJECT_SOURCE_DIR}/
)

include_directories(
    ${PROJECT_SOURCE_DIR}/
    ${IR_LIBRARY_PATH}/
)

add_subdirectory(${IR_LIBRARY_PATH} ${CMAKE_CURRENT_BINARY_DIR}/38k_ir)

add_executable(${GENERATED_BINARY}.elf
    main.c
    hw.c
    ring.c
)

target_link_libraries(${GENERATED_BINARY}.elf
    necrx
    panasonicrx
    samsungrx
)


add_custom_target(hex ALL
    COMMAND ${PREFIX}objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex ${GENERATED_BINARY}.elf ${GENERATED_BINARY}.hex
    DEPENDS ${GENERATED_BINARY}.elf
)

add_custom_target(load
    COMMAND avrdude -p ${DEVICE} -P ${TTY_DEVICE} -b ${TTY_BAUDRATE} -c wiring -U "flash:w:${GENERATED_BINARY}.hex:i" -D
    DEPENDS ${GENERATED_BINARY}.hex
)

add_custom_target(verify
    COMMAND avrdude -p ${DEVICE} -P ${TTY_DEVICE} -b ${TTY_BAUDRATE} -c wiring -U "flash:v:${GENERATED_BINARY}.hex:i" -D
    DEPENDS ${GENERATED_BINARY}.hex
)

add_custom_target(size
    COMMAND ${PREFIX}size ${GENERATED_BINARY}.elf
    DEPENDS ${GENERATED_BINARY}.elf
)
