
cmake_minimum_required(VERSION 3.1)
project(gui_example C ASM)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. gui)

zxgui_generate_scenes(${CMAKE_CURRENT_SOURCE_DIR}/src example_gui_scenes)
zxgui_generate_tiles(${CMAKE_CURRENT_SOURCE_DIR}/example_tiles.png example_tiles_gen)

add_executable(gui_example src/main.c)
target_link_options(gui_example PUBLIC -create-app -m)
target_link_libraries(gui_example PUBLIC ${ZXGUI_LIBRARIES})
add_dependencies(gui_example zxgui)

add_custom_target(gui_example.tap
    COMMAND mkdir -p "${CMAKE_CURRENT_SOURCE_DIR}/build"
    COMMAND cp "${CMAKE_CURRENT_BINARY_DIR}/gui_example.tap" "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS gui_example)