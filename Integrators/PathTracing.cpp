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

    double pdf_dirLight = 0.0f;
    double pdf_indirLight = 0.0f;
    double pdf_sum = 0.0f; // MIS的总pdf

    Ray directRay(p + N * eps, ws);

    Intersection directRayInter = pScene->intersect(directRay);

    if (directRayInter.happened && (x - directRayInter.position).norm() < eps) {
        L_dir = ws_inter.emit.cwiseProduct(wo_inter.material->eval(-ws, -wo, N)) * N.dot(ws) * NN.dot(-ws) /
                ((x - p).squaredNorm() * pdf_light);
        if(fabs(wo_inter.material->pdf(-ws, -wo, N)) < eps) { // 如果光线往这个方向散射的 pdf 为 0, 说明这个采样的 pdf 应该是0(采样光源只是从光源上找了个点, 不能保证物理上真的可以有一条这样的光线(有可能这条光线会被挡住, 也有可能光根本不可能散射向这个方向(比如完美反射/折射的方向是唯一的)))
            pdf_dirLight = 0.0f;
            pdf_sum += pdf_dirLight;
        }else {
            pdf_dirLight = (pdf_light * (x - p).squaredNorm()) / fabs(NN.dot(-ws)); // 蒙特卡洛积分的基本要求是要对谁积分就采样谁, 因此我们需要把对光源面积的采样变换为对立体角的采样
            // 本质上就是换了一种采样分布
            //std::cout << pdf_dirLight << std::endl;
            //if(std::fabs(pdf_dirLight - pdf_dirLight2) > eps) throw std::runtime_error("Failed");
            pdf_sum += pdf_dirLight;
        }
    }

    float ksi = get_random_float();

    if (ksi > uRussianRoulette) {
        L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
        pdf_indirLight = (1 - uRussianRoulette); // 死于RR了不代表这不是路径的一部分, 也是需要加权的
        pdf_sum += pdf_indirLight;
    } else {
        Eigen::Vector3f wi = wo_inter.material->sample(wo, N);

        Eigen::Vector3f ro; // r origin
        if (wi.dot(N) > 0.0f) { // 加个偏移避免自相交`
            ro = p + N * eps;
        } else {
            ro = p - N * eps;
        }
        Ray r(ro, wi);

        Intersection indirectRayInter = pScene->intersect(r);
        if (!indirectRayInter.happened) {
            L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
            pdf_indirLight = wo_inter.material->pdf(wo, wi, N) * uRussianRoulette; // 没打到东西也是需要加权的
            pdf_sum += pdf_indirLight;
        } else {
            //if (!indirectRayInter.material->hasEmission()) { // MIS混合了直接和间接光照, 因此即使同时采样BSDF以及光源, 接收到的能量也不会强于光源强度
                L_indir = CastRay(r, pScene, depth + 1).cwiseProduct(wo_inter.material->eval(-wi, -wo, N)) *
                          N.dot(wi) /
                          (wo_inter.material->pdf(wo, wi, N) * uRussianRoulette);
                pdf_indirLight = wo_inter.material->pdf(wo, wi, N) * uRussianRoulette;
                pdf_sum += pdf_indirLight;
            //}
        }
    }


    if (fabs(pdf_sum) > 1e-8) { // 添加多重重要性采样的权重 TODO: 这个地方判断pdf_sum目前有精度问题(比如面光源采样一个点的概率其实很小)
        L_dir = L_dir * (pdf_dirLight / pdf_sum);
        L_indir = L_indir * (pdf_indirLight / pdf_sum);
    } else {
        L_dir *= 0.0f;
        L_indir *= 0.0f;
        //std::runtime_error("No Sampling Approach Available");
    }

/*
    if (wo_inter.material->getType() != Mirror && wo_inter.material->getType() != Glass) {
        Ray directRay(p + N * eps, ws);

        Intersection directRayInter = pScene->intersect(directRay);
        //\if(directRayInter.happened) puts("OK");

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

            Eigen::Vector3f ro; // r origin
            if(wi.dot(N) > 0.0f){ // 加个偏移避免自相交`
                ro = p + N * eps;
            }else {
                ro = p - N * eps;
            }
            Ray r(ro, wi);

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
        //puts("Hit Glass");
        float ksi = get_random_float();

        if (ksi > uRussianRoulette) {
            L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
        } else {
            Eigen::Vector3f wi = wo_inter.material->sample(wo, N);

            Eigen::Vector3f ro; // r origin
            if(wi.dot(N) > 0.0f){ // 加个偏移避免自相交`
                ro = p + N * eps;
            }else {
                ro = p - N * eps;
            }
            Ray r(ro, wi);

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
    */
    return wo_inter.material->getEmission() + L_dir + L_indir; // 最终的着色为材质自己的发光 + 直接光照 + 间接光照
}

Eigen::Vector3f
PathTracingIntegrator::Li(const Ray &ray, const Scene *pScene) { // 返回 ray 与 pScene 指向的场景的交点向 -ray 方向发出的 radiance
    return CastRay(ray, pScene, 0);
}