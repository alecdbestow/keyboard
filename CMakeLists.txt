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
add_subdirectory(regex)
#add_library(regex STATIC)
#target_sources(regex PUBLIC 
#    ${CMAKE_CURRENT_LIST_DIR}/re.c
#)
#target_link_libraries(regex PUBLIC pico_stdlib)


add_library(string_help STATIC)
target_sources(string_help PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/string_help.c
)
target_link_libraries(string_help PUBLIC pico_stdlib)

add_library(dict STATIC)
target_include_directories(dict INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/FatFs_SPI/sd_driver
)
target_sources(dict PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/dictionary.c
    ${CMAKE_CURRENT_LIST_DIR}/dictionary.h
)
target_link_libraries(dict PUBLIC pico_stdlib FatFs_SPI)


add_library(keyboard_reader STATIC)
target_include_directories(keyboard_reader PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
)
target_sources(keyboard_reader PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/keyboard_reader.c
)
target_link_libraries(keyboard_reader PUBLIC pico_stdlib tinyusb_device tinyusb_board)



add_library(stroke STATIC)
target_include_directories(stroke INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_sources(stroke PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/stroke.c
)
target_link_libraries(stroke PUBLIC pico_stdlib)


add_library(translation STATIC)
target_sources(translation PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/translation.c
)
target_link_libraries(translation PUBLIC pico_stdlib stroke)


add_library(outputter STATIC)
target_sources(outputter PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/outputter.c
)
target_link_libraries(outputter PUBLIC pico_stdlib translation string_help)


add_library(meta STATIC)
target_sources(meta PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/meta.c
)
target_link_libraries(meta PUBLIC pico_stdlib match)


add_library(stroke_stream STATIC)
target_include_directories(stroke_stream PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/FatFs_SPI/sd_driver
)
target_sources(stroke_stream PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/stroke_stream.c
)
target_link_libraries(stroke_stream PUBLIC pico_stdlib dict meta translation outputter)



add_library(typer STATIC)
target_include_directories(typer PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/FatFs_SPI/sd_driver
)
target_sources(typer PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/typer.c
    ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
)
target_link_libraries(typer PUBLIC pico_stdlib tinyusb_device tinyusb_board stroke_stream)






add_executable(main)
target_include_directories(main PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
)
target_sources(main PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)
target_link_libraries(main PUBLIC regex pico_stdlib typer stroke_stream tinyusb_device tinyusb_board keyboard_reader stroke)

pico_add_extra_outputs(main)