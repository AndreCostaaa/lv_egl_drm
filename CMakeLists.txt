cmake_minimum_required(VERSION 3.25.1)
project(lv_egl_drm LANGUAGES C CXX)
cmake_policy(SET CMP0079 NEW)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ggdb")

find_package(PkgConfig REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(LV_BUILD_CONF_PATH
    "${CMAKE_BINARY_DIR}/lv_conf.h"
    CACHE PATH "path to lv_conf.h" FORCE)

set(LVGL_TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/lvgl/lv_conf_template.h")
set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/lv_conf_2d.defaults")
set(GENERATE_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/lvgl/scripts/generate_lv_conf.py")

configure_file(${LVGL_TEMPLATE_PATH} ${CMAKE_BINARY_DIR}/lv_conf_template.h
               COPYONLY)
configure_file(${LV_CONF_DEFAULTS_PATH} ${CMAKE_BINARY_DIR}/lv_conf.defaults
               COPYONLY)
configure_file(${GENERATE_SCRIPT_PATH} ${CMAKE_BINARY_DIR}/generate_lv_conf.py
               COPYONLY)

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

set(LV_BUILD_SET_CONFIG_OPTS
    ON
    CACHE BOOL "create cmake variables from lv_conf_internal.h")

add_subdirectory(lvgl)

if(CONFIG_LV_USE_GLTF)
  message(STATUS "Compiling with GLTF support")
  include(FetchContent)

  FetchContent_Declare(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf
    GIT_TAG 4e2261350888bae7c35a1f39991f6233d57795f5)

  set(FASTGLTF_ENABLE_DEPRECATED_EXT
      ON
      CACHE BOOL "" FORCE)

  set(FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
      ON
      CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(fastgltf)

  set(WEBP_BUILD_ANIM_UTILS
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_CWEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_DWEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_GIF2WEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_IMG2WEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_VWEBP
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_WEBPINFO
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_WEBPMUX
      OFF
      CACHE BOOL "" FORCE)
  set(WEBP_BUILD_EXTRAS
      OFF
      CACHE BOOL "" FORCE)

  FetchContent_Declare(
    webp
    GIT_REPOSITORY https://github.com/webmproject/libwebp
    GIT_TAG fa6f56496a442eed59b103250021e4b14ebf1427)

  FetchContent_MakeAvailable(webp)
  target_link_libraries(lvgl PUBLIC fastgltf webp)
endif()

if(CONFIG_LV_USE_EVDEV)
  message(STATUS "Compiling with EVDEV support")
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(EVDEV REQUIRED libevdev)

  target_link_libraries(lvgl PUBLIC ${EVDEV_LIBRARIES})
  target_include_directories(lvgl PUBLIC ${EVDEV_INCLUDE_DIRS})
endif()

pkg_check_modules(LIBDRM REQUIRED libdrm)
pkg_check_modules(GBM REQUIRED gbm)

target_include_directories(
  lvgl PUBLIC ${LIBDRM_INCLUDE_DIRS} ${GBM_INCLUDE_DIRS} ${CMAKE_BINARY_DIR} dl)

add_executable(lv_egl_drm src/main.c src/mouse_cursor_icon.c)
target_link_libraries(lv_egl_drm PUBLIC lvgl lvgl_demos m ${LIBDRM_LIBRARIES}
                                        ${GBM_LIBRARIES} dl)

set_target_properties(lv_egl_drm PROPERTIES COMPILE_DEFINITIONS
                                            "${LVGL_COMPILER_DEFINES}")
