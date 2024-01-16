//
// Created by yizr_cnyali on 2023/12/3.
//

#include "Eigen/Dense"

#ifndef YZGI_RAY_HPP
#define YZGI_RAY_HPP

struct Ray { // now = origin + direction * t;
    Eigen::Vector3f origin; // 光线起点
    Eigen::Vector3f direction, direction_inv; // 光线方向和光线方向的逆
    double t; // 当前光线传输的时长
    double t_min, t_max; // 光线传输的时长范围

    Ray(const Eigen::Vector3f &ori, const Eigen::Vector3f &dir, const double _t = 0.0) : origin(ori), direction(dir),
                                                                                         t(_t) {

        direction_inv = Eigen::Vector3f(1. / direction.x(), 1. / direction.y(), 1. / direction.z());
        t_min = 0.0;
        t_max = std::numeric_limits<double>::max();

    }

    Eigen::Vector3f operator()(double t) const { return origin + direction * t; }

    friend std::ostream &operator<<(std::ostream &os, const Ray &r) {

        os << "[origin:=" << r.origin << ", direction=" << r.direction << ", time=" << r.t << "]\n";
        return os;
    }
};

#endif //YZGI_RAY_HPP
