cmake_minimum_required(VERSION 3.25.1)
project(lv_egl_drm LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ggdb")

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(LV_BUILD_CONF_PATH
    "${CMAKE_BINARY_DIR}/lv_conf.h"
    CACHE PATH "path to lv_conf.h" FORCE)

set(LVGL_TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/lvgl/lv_conf_template.h")
set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/lv_conf.defaults")
set(GENERATE_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/lvgl/scripts/generate_lv_conf.py")

configure_file(${LVGL_TEMPLATE_PATH} ${CMAKE_BINARY_DIR}/lv_conf_template.h
               COPYONLY)
configure_file(${LV_CONF_DEFAULTS_PATH} ${CMAKE_BINARY_DIR}/lv_conf.defaults
               COPYONLY)
configure_file(${GENERATE_SCRIPT_PATH} ${CMAKE_BINARY_DIR}/generate_lv_conf.py
               COPYONLY)

message(STATUS "Generating lv_conf.h from defaults")
execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${LV_BUILD_CONF_PATH}
                RESULT_VARIABLE touch_result)

execute_process(
  COMMAND
    ${Python3_EXECUTABLE} ${GENERATE_SCRIPT_PATH} --template
    ${LVGL_TEMPLATE_PATH} --defaults ${LV_CONF_DEFAULTS_PATH} --config
    ${LV_BUILD_CONF_PATH}
  RESULT_VARIABLE config_result
  OUTPUT_VARIABLE config_output
  ERROR_VARIABLE config_error)

if(NOT config_result EQUAL 0)
  message(FATAL_ERROR "Failed to generate lv_conf.h: ${config_error}")
endif()

message(STATUS "lv_conf.h generated successfully")

set(CONFIG_LV_BUILD_EXAMPLES
    OFF
    CACHE BOOL "disable lvgl examples" FORCE)

set(CONFIG_LV_USE_THORVG_INTERNAL
    OFF
    CACHE BOOL "disable thorvg internal" FORCE)

set(CONFIG_LV_BUILD_DEMOS
    ON
    CACHE BOOL "enable lvgl demos" FORCE)
add_subdirectory(lvgl)

pkg_check_modules(LIBDRM REQUIRED libdrm)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(GLESV2 REQUIRED glesv2)
pkg_check_modules(GBM REQUIRED gbm)

target_include_directories(
  lvgl PUBLIC ${EGL_INCLUDE_DIRS} ${GLESV2_INCLUDE_DIRS} ${LIBDRM_INCLUDE_DIRS}
              ${GBM_INCLUDE_DIRS} ${CMAKE_BINARY_DIR})

add_executable(lv_egl_drm src/main.c)
target_link_libraries(
  lv_egl_drm
  PUBLIC lvgl
         lvgl_demos
         m
         ${EGL_LIBRARIES}
         ${GLESV2_LIBRARIES}
         ${LIBDRM_LIBRARIES}
         ${GBM_LIBRARIES})

set_target_properties(lv_egl_drm PROPERTIES COMPILE_DEFINITIONS
                                            "${LVGL_COMPILER_DEFINES}")
