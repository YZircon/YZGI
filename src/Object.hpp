//
// Created by yizr_cnyali on 2023/12/18.
//
#pragma once
#ifndef YZGI_OBJECT_HPP
#define YZGI_OBJECT_HPP

#include "Intersection.hpp"
#include "Ray.hpp"
#include "Bounds3.hpp"

class Object {
public:
    Object() {}

    virtual ~Object() {}

    virtual bool intersect(Ray ray) = 0;

    virtual Intersection getIntersection(Ray ray) = 0;

    virtual void sample(Intersection& inter, float& pdf) = 0;

    virtual float getArea() = 0;

    virtual bool hasEmit() = 0;
};

#endif //YZGI_OBJECT_HPP
