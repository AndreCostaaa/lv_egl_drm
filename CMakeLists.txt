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

add_executable(demo src/main.c)

pkg_check_modules(LIBDRM REQUIRED libdrm)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(GLESV2 REQUIRED glesv2)
pkg_check_modules(GBM REQUIRED gbm)

target_include_directories(
  lvgl PUBLIC ${EGL_INCLUDE_DIRS} ${GLESV2_INCLUDE_DIRS} ${LIBDRM_INCLUDE_DIRS}
              ${GBM_INCLUDE_DIRS})
target_link_libraries(
  demo
  PUBLIC lvgl
         lvgl_demos
         m
         ${EGL_LIBRARIES}
         ${GLESV2_LIBRARIES}
         ${LIBDRM_LIBRARIES}
         ${GBM_LIBRARIES})

set_target_properties(demo PROPERTIES COMPILE_DEFINITIONS
                                      "${LVGL_COMPILER_DEFINES}")
