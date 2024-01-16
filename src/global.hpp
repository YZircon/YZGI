//
// Created by yizr_cnyali on 2024/1/12.
//
#pragma once
#ifndef YZGI_GLOBAL_HPP
#define YZGI_GLOBAL_HPP

#include <random>
#include <iostream>
#include "Eigen/Dense"

constexpr float eps = 1e-4;

inline float clamp(const float &lo, const float &hi, const float &v) { return std::max(lo, std::min(hi, v)); }

inline float get_random_float() {// 生成一个[0,1]范围内的随机实数
    static std::random_device dev;
    static std::mt19937 rng(dev());  // 在 Linux/Unix 上使用 random_device 时，会使用操作系统提供的熵池，当熵池耗尽以后，random_device 会将调用阻塞，直至有足够的熵可以使用为止, 因此有一个巨大的时间开销
    static std::uniform_real_distribution<float> gen(0.f, 1.f); 

    return gen(rng);
}

inline void UpdateProgress(float progress){
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

#endif //YZGI_GLOBAL_HPP
