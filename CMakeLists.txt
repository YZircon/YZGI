cmake_minimum_required(VERSION 3.24)
project(YZGI)

set(CMAKE_CXX_STANDARD 17)

include_directories(3rdparty/eigen)
include_directories(3rdparty/OBJ-Loader)
include_directories(3rdparty/stb)

add_executable(YZGI Core/main.cpp
        Core/Renderer.cpp
        Core/Renderer.hpp
        Core/Material.cpp
        Core/Material.hpp
        Core/Texture.cpp
        Core/Texture.hpp
        Core/Sampler.cpp
        Core/Sampler.hpp
        Core/Scene.cpp
        Core/Scene.hpp
        Core/Ray.hpp
        Core/Object.hpp
        Core/Intersection.hpp
        Core/Triangle.hpp
        Core/Bounds3.hpp
        Core/global.hpp
        DS/BVH.cpp
        DS/BVH.hpp Integrators/PathTracing.cpp Integrators/PathTracing.hpp Core/Integrator.hpp Integrators/PhotonMapping.cpp Integrators/PhotonMapping.hpp)
