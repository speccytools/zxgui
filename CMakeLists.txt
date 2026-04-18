cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE GUI_ASM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm")
file(GLOB_RECURSE GUI_C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(zxgui OBJECT ${GUI_C_SOURCES} ${GUI_ASM_SOURCES})
target_include_directories(zxgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# usage example
# zxgui_generate_scenes(${CMAKE_CURRENT_SOURCE_DIR}/src zxgui_scenes)
function(zxgui_generate_scenes)
    add_custom_target(${ARGV1} ALL
        COMMAND python3 ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tools/guigen.py --source ${ARGV0} --suffix inc.h
        WORKING_DIRECTORY ${ARGV0})
    add_dependencies(zxgui ${ARGV1})
endfunction()


# usage example
# zxgui_generate_tiles(${CMAKE_CURRENT_SOURCE_DIR}/data/gui.png)
function(zxgui_generate_tiles)
    add_custom_target(zxgui_tiles_gen ALL
        COMMAND python3 ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tools/png2c.py ${ARGV0} --no-print-string --id=zxgui_tiles --preferred-bg=black > ${CMAKE_CURRENT_BINARY_DIR}/zxgui_tiles.h)
    add_library(zxgui_tiles OBJECT ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tiles/zxgui_tiles.c)
    add_dependencies(zxgui_tiles zxgui_tiles_gen)
    add_dependencies(zxgui zxgui_tiles)
    target_include_directories(zxgui_tiles PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

# usage example
# zxgui_generate_image(${CMAKE_CURRENT_SOURCE_DIR}/img/splash.png splash)
function(zxgui_generate_image IMAGE NAME)
    set(IMAGE_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.h)
    add_custom_command(
        OUTPUT ${IMAGE_HEADER}
        COMMAND /bin/sh -c "python3 \"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tools/png2c.py\" \"${IMAGE}\" --no-print-string --dump-color --id=${NAME} --preferred-bg=black > \"${IMAGE_HEADER}\""
        DEPENDS ${IMAGE} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tools/png2c.py
        VERBATIM
    )
    add_custom_target(${NAME}_image_gen ALL DEPENDS ${IMAGE_HEADER})
endfunction()

set(ZXGUI_LIBRARIES zxgui zxgui_tiles CACHE STRING "A list of zxgui libraries")
