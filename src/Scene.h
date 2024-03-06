//
// Created by yizr_cnyali on 2023/12/3.
//
#pragma once

#include "Eigen/Dense"
#include "Ray.hpp"
#include "Object.hpp"
#include "Intersection.hpp"
#include "BVH.hpp"

#ifndef YZGI_SCENE_H
#define YZGI_SCENE_H


class Scene {
public:
    int width = 784;
    int height = 784;
    double fov = 40;
    float RussianRoulette = 0.8;

    Eigen::Vector3f castRay(const Ray &ray, int depth) const;
    Intersection intersect(const Ray &ray) const;
    void sampleLight(Intersection& inter, float& pdf) const;
    void buildBVH();

    void add(Object* _object){
        objects.emplace_back(_object);
    }
private:

    std::vector<Object*> objects;
    BVH* bvh;
};


#endif //YZGI_SCENE_H
