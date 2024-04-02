//
// Created by yizr_cnyali on 2024/3/23.
//
#pragma once
#ifndef YZGI_PATHTRACING_HPP
#define YZGI_PATHTRACING_HPP

#include <Eigen/Dense>
#include "../Core/Ray.hpp"
#include "../Core/Scene.hpp"
#include "../Core/Integrator.hpp"

class PathTracingIntegrator : public Integrator{
public:
    PathTracingIntegrator(int depth, double RR) {
        uMaxDepth = depth;
        uRussianRoulette = RR;
    }

    ~PathTracingIntegrator() = default;

    Eigen::Vector3f Li(const Ray &ray, const Scene *pScene) override;

    Eigen::Vector3f CastRay(const Ray &ray, const Scene *pScene, int depth);

private:
    unsigned int uMaxDepth; // 最大的递归深度
    double uRussianRoulette; // 光线通过 RR 测试存活下来的概率
};


#endif //YZGI_PATHTRACING_HPP
