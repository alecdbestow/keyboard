

add_library(regex STATIC)
target_sources(regex PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/regex-dfa.c
    ${CMAKE_CURRENT_LIST_DIR}/regex-nfa.c
    ${CMAKE_CURRENT_LIST_DIR}/regex.c
)
pico_set_binary_type(regex no_flash)
target_link_libraries(regex PUBLIC pico_stdlib)
