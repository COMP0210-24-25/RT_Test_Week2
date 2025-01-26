cmake_minimum_required(VERSION 3.16)
project(RayTracingTesting
  VERSION 0.0.1
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 3 REQUIRED)
enable_testing() 

add_executable(TestRayTracing ../source/Ray.cpp ../source/Rendering.cpp ../source/Sphere.cpp ../source/VectorUtils.cpp ../source/IntersectionData.cpp ../source/Material.cpp ../source/Camera.cpp testWeek2.cpp)
target_include_directories(TestRayTracing PUBLIC ${CMAKE_SOURCE_DIR}/../include/)
target_link_libraries(TestRayTracing PUBLIC Catch2::Catch2WithMain)
