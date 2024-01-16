cmake_minimum_required(VERSION 3.24)
project(YZGI)

set(CMAKE_CXX_STANDARD 17)

include_directories(3rdparty/eigen)
include_directories(3rdparty/OBJ-Loader)
include_directories(3rdparty/stb)

add_executable(YZGI src/main.cpp src/Renderer.cpp src/Renderer.h src/Material.cpp src/Material.h src/Texture.cpp src/Texture.h src/Sampler.cpp src/Sampler.h src/Scene.cpp src/Scene.h src/Ray.hpp src/Object.hpp src/Intersection.hpp src/Triangle.hpp src/Bounds3.hpp src/BVH.cpp src/BVH.h src/global.hpp)
