//
// Created by yizr_cnyali on 2023/12/18.
//
#pragma once

#ifndef YZGI_BOUNDS3_HPP
#define YZGI_BOUNDS3_HPP

#include "Eigen/Dense"
#include "Ray.hpp"
#include "global.hpp"

class Bounds3{
public:
    Eigen::Vector3f pMin, pMax; // 包围盒, TODO: 需要支持 1.合并 2. 判断是否和光线存在交点

    Bounds3(){
        float minNum = std::numeric_limits<float>::lowest();
        float maxNum = std::numeric_limits<float>::max();

        pMin = Eigen::Vector3f(minNum, minNum, minNum);
        pMax = Eigen::Vector3f(maxNum, maxNum, maxNum);
    }

    Bounds3(const Eigen::Vector3f p) : pMin(p), pMax(p) {}

    Bounds3(const Eigen::Vector3f& p1, const Eigen::Vector3f& p2){
        pMin = Eigen::Vector3f(std::fmin(p1.x(), p2.x()), std::fmin(p1.y(), p2.y()), std::fmin(p1.z(), p2.z()));
        pMax = Eigen::Vector3f(std::fmax(p1.x(), p2.x()), std::fmax(p1.y(), p2.y()), std::fmax(p1.z(), p2.z()));
    }

    Eigen::Vector3f Diagonal() const { return pMax - pMin; }

    int maxExtent() const{ // 找到最长轴, 从此轴心划分
        Eigen::Vector3f d = Diagonal();
        if (d.x() > d.y() && d.x() > d.z())
            return 0;
        else if (d.y() > d.z())
            return 1;
        else
            return 2;
    }

    Eigen::Vector3f Centroid(){ // 包围盒中心
        return 0.5 * pMin + 0.5 * pMax;
    }

    Bounds3 Union(const Bounds3& _b){
        Bounds3 ret;
        ret.pMin = pMin.cwiseMin(_b.pMin);
        ret.pMax = pMax.cwiseMax(_b.pMax);
        return ret;
    }

    Bounds3 Union(const Eigen::Vector3f& _b){
        Bounds3 ret;
        ret.pMin = pMin.cwiseMin(_b);
        ret.pMax = pMax.cwiseMax(_b);
        return ret;
    }

    inline bool IntersectP(const Ray& ray, const Eigen::Vector3f& invDir) const;

};

inline bool Bounds3::IntersectP(const Ray &ray, const Eigen::Vector3f &invDir) const { // 在头文件外面实现成员函数, 如果不 inline 内联展开, 多次引用该头文件会导致此函数被重复定义
    // invDir: ray direction(x,y,z), invDir=(1.0/x,1.0/y,1.0/z), use this because Multiply is faster that Division
    // TODO test if ray bound intersects
    // 利用轴对齐包围盒的性质

    float tenter = std::numeric_limits<float>::lowest(), texit = std::numeric_limits<float>::max();

    // lowest 是 float 能去到的最小值, min 是绝对值最小的 float

    Eigen::Vector3f f1(pMin.x(), pMin.y(), pMin.z()), f2(pMax.x(), pMax.y(), pMax.z()); // 一对平行 yOz, xOz, xOy 平面的一维坐标 (因为平行于一个平面所以只用垂直方向维度)
    Eigen::Vector3f t1 = (f1 - ray.origin).cwiseProduct(invDir), t2 = (f2 - ray.origin).cwiseProduct(invDir);

    tenter = std :: max({std :: min(t1.x(), t2.x()), std :: min(t1.y(), t2.y()), std :: min(t1.z(), t2.z())});
    texit = std :: min({std :: max(t1.x(), t2.x()), std :: max(t1.y(), t2.y()), std :: max(t1.z(), t2.z())});
    if((tenter < texit || fabs(tenter - texit) < eps) && (texit > 0 || fabs(texit) < eps)) return true; // tenter <= texit && texit >= 0
    return false;
}

#endif //YZGI_BOUNDS3_HPP
