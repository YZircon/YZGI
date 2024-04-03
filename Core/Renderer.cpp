//
// Created by yizr_cnyali on 2023/12/3.
//

#include <cstdio>
#include <chrono>
#include "Renderer.hpp"
#include "Ray.hpp"
#include "global.hpp"
#include "Integrator.hpp"

inline float deg2rad(const float &deg) { return deg * M_PI / 180.0; }

void Renderer::Render(const Scene *pScene, Integrator *integrator) {
    std::vector<Eigen::Vector3f> framebuffer(pScene->width * pScene->height);

    float scale = tan(deg2rad(pScene->fov * 0.5));
    float imageAspectRatio = pScene->width / (float) pScene->height;
    Eigen::Vector3f eye_pos(278, 273, -800);
    int m = 0;

    // change the spp value to change sample ammount
    int spp = 1024;
    std::cout << "SPP: " << spp << "\n";
    for (uint32_t j = 0; j < pScene->height; ++j) {
        for (uint32_t i = 0; i < pScene->width; ++i) {
            // generate primary ray direction
            float x = (2 * (i + 0.5) / (float) pScene->width - 1) *
                      imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float) pScene->height) * scale;

            Eigen::Vector3f dir = Eigen::Vector3f(-x, y, 1).normalized();
            auto start = std::chrono::steady_clock::now();
            for (int k = 0; k < spp; k++) {
                framebuffer[m] += integrator->Li(Ray(eye_pos, dir), pScene) /
                spp; // TODO: 提供一个通用函数(Integrators, 因为本质上都是渲染方程的积分器)计算每个点的颜色, 视口和相机数据放在 scene 里一起传入
                //std::cout << "M = " << m << framebuffer[m] << std :: endl;
                //std :: cout << scene.castRay(Ray(eye_pos, dir), 0) << std :: endl;
            }
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(
                    end - start);
            //std::cout << "Elapsed time: " << duration.count() << " seconds/pixel\n";
            m++;
        }
        UpdateProgress(j / (float) pScene->height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
    FILE *fp = fopen("binary.ppm", "wb");
    (void) fprintf(fp, "P6\n%d %d\n255\n", pScene->width, pScene->height);
    for (auto i = 0; i < pScene->height * pScene->width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char) (255 * std::pow(clamp(0, 1, framebuffer[i].x()), 0.6f));
        color[1] = (unsigned char) (255 * std::pow(clamp(0, 1, framebuffer[i].y()), 0.6f));
        color[2] = (unsigned char) (255 * std::pow(clamp(0, 1, framebuffer[i].z()), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
    return;
}