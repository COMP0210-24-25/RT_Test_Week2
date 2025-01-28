cmake_minimum_required(VERSION 3.16)
project(RayTracingTesting
  VERSION 0.0.1
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3 REQUIRED)
enable_testing() 

file(GLOB SOURCE_FILES ../source/*.cpp)
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*main.cpp$")
add_executable(TestRayTracing ${SOURCE_FILES} testWeek2.cpp)
target_include_directories(TestRayTracing PUBLIC ${CMAKE_SOURCE_DIR}/../include/)
target_link_libraries(TestRayTracing PUBLIC Catch2::Catch2WithMain)
