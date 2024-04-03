//
// Created by yizr_cnyali on 2023/12/3.
//

#pragma once

#ifndef YZGI_MATERIAL_HPP
#define YZGI_MATERIAL_HPP

#include <iostream>
#include "Eigen/Dense"

#include "global.hpp"

enum MaterialType {
    DIFFUSE, Mirror, Glass, Microfacet
}; // TODO : 本质就是实现一个 BSDF, 短期内先写完 DIFFUSE 的, 整个系统没问题了再去扩展

class Material {
public:
    Material(MaterialType _type, Eigen::Vector3f _Emission);

    ~Material();

    Eigen::Vector3f sample(const Eigen::Vector3f &wi, const Eigen::Vector3f &N); // TODO : 采样反射光

    Eigen::Vector3f
    eval(const Eigen::Vector3f &wi, const Eigen::Vector3f &wo,
         const Eigen::Vector3f &N); // 获取 BSDF 值, wi: 入射方向, wo: 出射方向, N: 表面法向

    float pdf(const Eigen::Vector3f &wi, const Eigen::Vector3f &wo,
              const Eigen::Vector3f &N); //采样的 PDF, wi: 入射方向, wo: 出射方向, N: 表面法向

    bool hasEmission() {
        return Emission.norm() > eps;
    }

    MaterialType getType();

    std::string getName();

    Eigen::Vector3f getEmission(); // 材质自身的发光

    Eigen::Vector3f Kd; // diffuse 值

    Eigen::Vector3f Ks; // specular 值

    Eigen::Vector3f F0; // Fresnel 项的初始值 F0

    float roughness;

private:

    MaterialType materialType = DIFFUSE; // 材质类型

    Eigen::Vector3f Emission; // 自身发光

    std::string name; // 名字

    Eigen::Vector3f
    Fresnel(const Eigen::Vector3f &i, const Eigen::Vector3f &N); // 这个地方是i和半程向量h的夹角还是和法线的夹角? 有说法说这两个是一样的, 对吗？

    float Geometry1(const Eigen::Vector3f &x, const Eigen::Vector3f &h, const Eigen::Vector3f &N);

    float GeometryGGX(const Eigen::Vector3f &i, const Eigen::Vector3f &o, const Eigen::Vector3f &h,
                      const Eigen::Vector3f &N); // i 为入射光方向(的反方向), o 为出射光方向, h 为 i 和 o 的半程向量方向

    float DistributionGGX(const Eigen::Vector3f &h, const Eigen::Vector3f &N);

};


#endif //YZGI_MATERIAL_HPP
