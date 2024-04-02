//
// Created by yizr_cnyali on 2023/12/3.
//

#pragma once

#ifndef YZGI_RENDERER_HPP
#define YZGI_RENDERER_HPP

#include "Scene.hpp"
#include "Integrator.hpp"

class Renderer {
public:
    void Render(const Scene *pScene, Integrator *integrator); // TODO : 这玩意的返回值应该是个矩阵形式存储的图像
};

#endif //YZGI_RENDERER_HPP
