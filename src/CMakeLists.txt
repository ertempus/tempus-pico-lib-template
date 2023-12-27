add_executable(${NAME}
    tempus_pico_puzzle_main.cpp
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${NAME}
    pico_stdlib
    tempus_pico_lib
)

target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${NAME} PRIVATE
    # CONFIG_TEMPUS_CLUESYSTEM_HOST="tempus.local"
)

pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${NAME})

add_custom_command(TARGET ${NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${PICO_GCC_TRIPLE}-size --format=gnu $<TARGET_FILE_NAME:${NAME}>
    COMMAND ${CMAKE_COMMAND} -E echo ""
    )
