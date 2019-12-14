cmake_minimum_required(VERSION 3.5)
project(procgen VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB src
  "src/*.cpp"
  "imgui/*.cpp"
  )

add_executable(lsys ${src})

target_include_directories(lsys
  PUBLIC
  $<INSTALL_INTERFACE:includes>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cereal>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gsl>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/.>
  )

target_compile_features(lsys PRIVATE cxx_std_17)
target_compile_definitions(
  lsys
  PRIVATE
  GSL_THROW_ON_CONTRACT_VIOLATION
  $<$<CONFIG:Debug>: DEBUG_CHECKS>
  )

find_package(OpenGL REQUIRED)
#set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 REQUIRED COMPONENTS system window graphics)

target_link_libraries(lsys
  PUBLIC
  ${OPENGL_LIBRARIES}
  sfml-system sfml-window sfml-graphics
  $<$<PLATFORM_ID:Linux>:stdc++fs>
  )

install(
  TARGETS lsys
  RUNTIME
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  )
