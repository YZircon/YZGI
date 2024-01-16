//
// Created by yizr_cnyali on 2023/12/18.
//

#ifndef YZGI_BOUNDS3_HPP
#define YZGI_BOUNDS3_HPP

#include "Eigen/Dense"

class Bounds3{
public:
    Eigen::Vector3f pMin, pMax; // 包围盒, TODO: 需要支持 1.合并 2. 判断是否和光线存在交点

};

#endif //YZGI_BOUNDS3_HPP
