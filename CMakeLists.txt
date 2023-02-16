cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()


#add_compile_options(-Wall
#        -Werror
#        )
add_subdirectory(FatFs_SPI)

add_library(typer STATIC)
target_include_directories(typer PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/FatFs_SPI/sd_driver
)
target_sources(typer PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/typer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
)
target_link_libraries(typer PUBLIC pico_stdlib tinyusb_device tinyusb_board)



add_library(dict STATIC)
target_include_directories(dict INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/FatFs_SPI/sd_driver
)
target_sources(dict PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/dictionary.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dictionary.hpp
)
target_link_libraries(dict PUBLIC pico_stdlib FatFs_SPI)


add_library(meta STATIC)
target_sources(meta PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/meta.cpp
)
target_link_libraries(meta PUBLIC pico_stdlib FatFs_SPI)


add_library(action_stream STATIC)
target_include_directories(typer PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/FatFs_SPI/sd_driver
)
target_sources(action_stream PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/action_stream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stroke.cpp
    ${CMAKE_CURRENT_LIST_DIR}/circap.cpp
)
target_link_libraries(action_stream PUBLIC pico_stdlib dict meta)



add_executable(main)
target_sources(main PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_include_directories(main PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(main PUBLIC pico_stdlib typer action_stream)

pico_enable_stdio_uart(main 0)
pico_add_extra_outputs(main)