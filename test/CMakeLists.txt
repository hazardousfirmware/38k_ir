project(ProjectName)

cmake_minimum_required(VERSION 3.0)

set(IR_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/../")

link_directories(
    ${PROJECT_SOURCE_DIR}/
)

include_directories(
    ${PROJECT_SOURCE_DIR}/
    ${IR_LIBRARY_PATH}/
)

enable_testing()

add_subdirectory(${IR_LIBRARY_PATH} ${CMAKE_CURRENT_BINARY_DIR}/38k_ir)

link_libraries(
    necrx
    nectx
    panasonicrx
    panasonictx
    samsungrx
)

add_executable(rxtx_test
    rxtx_test.cpp
)

add_test(NAME rxtx_test COMMAND rxtx_test)

add_executable(panatest
    panatest.c
)

add_test(NAME panatest COMMAND panatest)

add_executable(samsung_test
    samsung_test.c
)

add_test(NAME samsung_test COMMAND samsung_test)
