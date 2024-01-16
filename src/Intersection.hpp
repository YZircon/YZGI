//
// Created by yizr_cnyali on 2023/12/3.
//

#ifndef YZGI_INTERSECTION_HPP
#define YZGI_INTERSECTION_HPP

#include "Eigen/Dense"
#include "Material.h"

class Object;

struct Intersection {
    Intersection() {

        happened = false; // 是否发生交点
        position = Eigen::Vector3f(); // 交点坐标
        normal = Eigen::Vector3f(); // 交点法向量
        texCoords = Eigen::Vector3f(); // 交点的纹理坐标
        distance = std::numeric_limits<double>::max(); // 从光线出发点到交点的距离
        object = nullptr; // 发生交点的物体
        material = nullptr; // 发生交点的材质
    }

    bool happened;
    Eigen::Vector3f position;
    Eigen::Vector3f texCoords;
    Eigen::Vector3f normal;
    Eigen::Vector3f emit;
    double distance;

    Object *object;
    Material *material;
};

#endif //YZGI_INTERSECTION_HPP
