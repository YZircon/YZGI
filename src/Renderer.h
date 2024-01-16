//
// Created by yizr_cnyali on 2023/12/3.
//

#include "Scene.h"

#ifndef YZGI_RENDERER_H
#define YZGI_RENDERER_H

namespace PathTracing {
    class Renderer {
    public:
        void Render(const Scene& scene); // TODO : 这玩意的返回值应该是个矩阵形式存储的图像
    };
}

namespace PhotonMapping{
    class Renderer{
    public:
        void Render(const Scene& scene); // TODO : 这玩意的返回值应该是个矩阵形式存储的图像
    };
}


#endif //YZGI_RENDERER_H
