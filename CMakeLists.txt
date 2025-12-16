cmake_minimum_required(VERSION 3.29.0)

project(spsq VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 12.1.0
)
FetchContent_MakeAvailable(fmt)
 
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(spsq ${SOURCES})

target_link_libraries(spsq PRIVATE fmt::fmt)
target_link_libraries(spsq
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
