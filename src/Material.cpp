//
// Created by yizr_cnyali on 2023/12/3.
//

#include "Material.h"
#include "global.hpp"

Material::Material(MaterialType _type, Eigen::Vector3f _Emission) {
    materialType = _type;
    Emission = _Emission;
}

Material::~Material() = default;

Eigen::Vector3f Material::getEmission() { return Emission; }

std::string Material::getName() { return name; }

Eigen::Vector3f transform(const Eigen:: Vector3f& target, const Eigen::Vector3f& N) { // 用罗德里格斯旋转公式构造一个以 N 为 z 轴的标准正交基, 然后把 target 变换过去, 使得采样出射方向的半球变成以 N为中心轴的上半球
    Eigen::Vector3f B, C;
    if(std::fabs(N.x()) > std::fabs(N.y())){ 
        C = Eigen::Vector3f(N.z(), 0.0f, -N.x()).normalized();
    }else{ 
        C = Eigen::Vector3f(0.0f, N.z(), -N.y()).normalized();
    }
    //B = C.cross(N).normalized();
    B = C.cross(N);
    return target.x() * B + target.y() * C + target.z() * N;
}

Eigen::Vector3f Material::sample(const Eigen::Vector3f& wi, const Eigen::Vector3f& N) {

    switch (materialType){
        case DIFFUSE: {
            float x_1 = get_random_float(), x_2 = get_random_float();
            float z = std::fabs(1.0 - 2.0f * x_1);
            float r = std::sqrt(1.0 - z * z), phi = 2 * M_PI * x_2;

            Eigen::Vector3f localRay = Eigen::Vector3f(r * std::cos(phi), r * std::sin(phi), z);
            return transform(localRay, N);

            break;
        }
    }
}

Eigen::Vector3f Material::eval(const Eigen::Vector3f& wi, const Eigen::Vector3f& wo, const Eigen::Vector3f& N) {

    switch (materialType) {
        case DIFFUSE: {
            if (wo.dot(N) > 0.0f) {
                return Kd / M_PI; // TODO: 对于 Texture 来说, 实质上只是不同位置的 Kd 由贴图确定, 那么我增加一个传入交点即可进行采样
            } else {
                return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
            }
            break;
        }
    }
}

float Material::pdf(const Eigen::Vector3f& wi, const Eigen::Vector3f& wo, const Eigen::Vector3f& N) {

    switch (materialType) {
        case DIFFUSE: {
            if (wo.dot(N) > 0.0f) { // Uniform sample upper sphere, pdf = 1 / 2\pi (单位球面积为4\pi, 只有上半球面有效为 2\pi)
                return 0.5f / M_PI;
            } else {
                return 0.0f;
            }
            break;
        }
    }
}