//
// Created by yizr_cnyali on 2023/12/3.
//

#ifndef YZGI_MATERIAL_H
#define YZGI_MATERIAL_H

#include <iostream>
#include "Eigen/Dense"

#include "global.hpp"

enum MaterialType {
    DIFFUSE, SPECULAR
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

    std::string getName();

    Eigen::Vector3f getEmission(); // 材质自身的发光

    Eigen::Vector3f Kd; // diffuse 值

private:

    MaterialType materialType = DIFFUSE; // 材质类型

    Eigen::Vector3f Emission; // 自身发光

    std::string name; // 名字
};


#endif //YZGI_MATERIAL_H
