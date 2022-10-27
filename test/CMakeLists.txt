project(ProjectName)

cmake_minimum_required(VERSION 3.0)

include_directories(
    ../
)

add_executable(rxtest
    ../panasonic_rx.c
    main.c
)
