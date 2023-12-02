cmake_minimum_required(VERSION 3.10)

include(GNUInstallDirs)

project(graph C)

set(GLFW_LIBRARY_TYPE "SHARED")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/glfw SYSTEM)

find_package(OpenGL REQUIRED)

add_library(plug SHARED
  src/plug.c
)
target_include_directories(plug SYSTEM PRIVATE libs/glfw/include)
set_target_properties(plug PROPERTIES PUBLIC_HEADER src/plug.h)
install(TARGETS plug
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
target_link_libraries(plug glfw OpenGL::GL)

add_executable(graph src/main.c)
target_include_directories(graph SYSTEM PRIVATE libs/glfw/include)
target_link_libraries(graph glfw OpenGL::GL)