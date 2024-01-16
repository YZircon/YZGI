//
// Created by yizr_cnyali on 2023/12/3.
//

#include <cstdio>
#include "Renderer.h"
#include "Ray.hpp"
#include "global.hpp"

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

namespace PathTracing{
    void Renderer :: Render(const Scene& scene) {
        std::vector<Eigen::Vector3f> framebuffer(scene.width * scene.height);

        float scale = tan(deg2rad(scene.fov * 0.5));
        float imageAspectRatio = scene.width / (float)scene.height;
        Eigen::Vector3f eye_pos(278, 273, -800);
        int m = 0;

       // change the spp value to change sample ammount
        int spp = 5;
        std::cout << "SPP: " << spp << "\n";
        for (uint32_t j = 0; j < scene.height; ++j) {
            for (uint32_t i = 0; i < scene.width; ++i) {
                // generate primary ray direction
                float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                         imageAspectRatio * scale;
               float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

                Eigen::Vector3f dir = Eigen::Vector3f(-x, y, 1).normalized();
                for (int k = 0; k < spp; k++){
                    framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;
                    //std::cout << "M = " << m << framebuffer[m] << std :: endl;
                    //std :: cout << scene.castRay(Ray(eye_pos, dir), 0) << std :: endl;
                }
                m++;
            }
            UpdateProgress(j / (float)scene.height);
        }
        UpdateProgress(1.f);

        // save framebuffer to file
        FILE* fp = fopen("binary.ppm", "wb");
        (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
        for (auto i = 0; i < scene.height * scene.width; ++i) {
            static unsigned char color[3];
            color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x()), 0.6f));
            color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y()), 0.6f));
            color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z()), 0.6f));
            fwrite(color, 1, 3, fp);
        }
        fclose(fp);   
        return;
    }
}

namespace PhotonMapping{
    void Renderer :: Render(const Scene& scene) {
        puts("Rendering using Phonton Mapping");
        return;
    }
}