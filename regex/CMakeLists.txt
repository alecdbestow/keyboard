add_library(regex STATIC)
target_sources(regex PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/regex.c
    ${CMAKE_CURRENT_LIST_DIR}/regex-dfa.c
    ${CMAKE_CURRENT_LIST_DIR}/regex-nfa.c

)
target_link_libraries(regex PUBLIC pico_stdlib)