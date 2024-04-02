//
// Created by yizr_cnyali on 2024/3/23.
//
#pragma once
#ifndef YZGI_INTEGRATOR_HPP
#define YZGI_INTEGRATOR_HPP

#include "Eigen/Core"
#include "Ray.hpp"
#include "Scene.hpp"

class Integrator {
public:
    Integrator() {}

    virtual ~Integrator() {}

    virtual Eigen::Vector3f Li(const Ray &ray, const Scene *pScene) = 0;
};


#endif //YZGI_INTEGRATOR_HPP
