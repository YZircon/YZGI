//
// Created by yizr_cnyali on 2024/3/23.
//

#include "PathTracing.hpp"


Eigen::Vector3f PathTracingIntegrator::CastRay(const Ray &ray, const Scene *pScene, int depth) {


    Intersection wo_inter = pScene->intersect(ray); // 先看当前光线打到了啥, intersect是求光线和场景的交点

    if (!wo_inter.happened || depth > uMaxDepth) { // 啥也没打到就下班了
        return Eigen::Vector3f(0.0, 0.0, 0.0);
    }

    Eigen::Vector3f L_dir(0.0, 0.0, 0.0), L_indir(0.0, 0.0, 0.0); // 直接光照和间接光照

    Intersection ws_inter; // 采样光源
    float pdf_light;
    pScene->sampleLight(ws_inter, pdf_light); // Intersection为采样到的点, pdf为本次采样的pdf

    Eigen::Vector3f x = ws_inter.position;
    Eigen::Vector3f p = wo_inter.position;
    Eigen::Vector3f wo = ray.direction;
    Eigen::Vector3f ws = (x - p).normalized();
    Eigen::Vector3f N = wo_inter.normal; // n
    Eigen::Vector3f NN = ws_inter.normal; // n'
    if (wo_inter.material->getType() != Mirror) {
        Ray directRay(p + N * eps, ws);

        Intersection directRayInter = pScene->intersect(directRay);
        //if(directRayInter.happened) puts("OK");

        if (directRayInter.happened && (x - directRayInter.position).norm() < eps) {
            L_dir = ws_inter.emit.cwiseProduct(wo_inter.material->eval(-ws, -wo, N)) * N.dot(ws) * NN.dot(-ws) /
                    ((x - p).squaredNorm() * pdf_light);
            //std :: cout << wo_inter.material->eval(-ws, -wo, N) << std :: endl;
            //puts("");
        }
        float ksi = get_random_float();

        if (ksi > uRussianRoulette) {
            L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
        } else {
            Eigen::Vector3f wi = wo_inter.material->sample(wo, N);
            Ray r(p, wi);

            Intersection indirectRayInter = pScene->intersect(r);
            if (!indirectRayInter.happened) {
                L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
            } else {
                if (!indirectRayInter.material->hasEmission()) {
                    L_indir = CastRay(r, pScene, depth + 1).cwiseProduct(wo_inter.material->eval(-wi, -wo, N)) *
                              N.dot(wi) /
                              (wo_inter.material->pdf(wo, wi, N) * uRussianRoulette);
                }
            }
        }
    } else {
        float ksi = get_random_float();

        if (ksi > uRussianRoulette) {
            L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
        } else {
            Eigen::Vector3f wi = wo_inter.material->sample(wo, N);
            Ray r(p, wi);

            Intersection indirectRayInter = pScene->intersect(r);
            if (!indirectRayInter.happened) {
                L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
            } else {
                L_indir = CastRay(r, pScene, depth + 1).cwiseProduct(wo_inter.material->eval(-wi, -wo, N)) *
                          N.dot(wi) /
                          (wo_inter.material->pdf(wo, wi, N) * uRussianRoulette);
            }
        }
    }
    return wo_inter.material->getEmission() + L_dir + L_indir; // 最终的着色为材质自己的发光 + 直接光照 + 间接光照
}

Eigen::Vector3f
PathTracingIntegrator::Li(const Ray &ray, const Scene *pScene) { // 返回 ray 与 pScene 指向的场景的交点向 -ray 方向发出的 radiance
    return CastRay(ray, pScene, 0);
}