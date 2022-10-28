project(ProjectName)

cmake_minimum_required(VERSION 3.0)

include_directories(
    ../
)

add_executable(test
    main.cpp
    ../panasonic_rx.c
    ../panasonic_tx.c
    ../nec_rx.c
    ../nec_tx.c
)

add_executable(test2
    test.c
    ../panasonic_rx.c
)
