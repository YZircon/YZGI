//
// Created by yizr_cnyali on 2023/12/3.
//

#include "Scene.h"
#include "global.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

void Scene::buildBVH(){
    this->bvh = new BVH(objects);
}

Intersection Scene::intersect(const Ray &ray) const{ // 最简单的方式是扫一遍场景中的所有object, 保留最近的交点.
    Intersection inter;
/*
    for(const auto obj : objects){
        Intersection now = obj->getIntersection(ray);
        if(now.happened && now.distance < inter.distance){
            inter = now;
        }
    }*/
    inter = this->bvh->Intersect(ray);

    return inter;
}

void Scene::sampleLight(Intersection& inter, float& pdf) const{
    float emit_area_sum = 0;
    for(uint32_t k = 0; k < objects.size(); k++){
        if(objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for(uint32_t k = 0; k < objects.size(); k++){
        if(objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if(emit_area_sum >= p){
                objects[k]->sample(inter, pdf);
                break;
            }
        }
    }
}


Eigen::Vector3f Scene::castRay(const Ray &ray, int depth) const { // 返回 ray 与场景交点向 -ray 方向发出的 radiance

    Intersection wo_inter = intersect(ray); // 先看当前光线打到了啥 TODO: intersect是求光线和场景的交点
    if (wo_inter.happened == false) { // 啥也没打到就下班了
        return Eigen::Vector3f(0.0, 0.0, 0.0);
    }

    Eigen::Vector3f L_dir(0.0, 0.0, 0.0), L_indir(0.0, 0.0, 0.0); // 直接光照和间接光照

    Intersection ws_inter; // 采样光源
    float pdf_light;
    sampleLight(ws_inter, pdf_light); // Intersection为采样到的点, pdf为本次采样的pdf

    Eigen::Vector3f x = ws_inter.position;
    Eigen::Vector3f p = wo_inter.position;
    Eigen::Vector3f wo = ray.direction;
    Eigen::Vector3f ws = (x - p).normalized();
    Eigen::Vector3f N = wo_inter.normal; // n
    Eigen::Vector3f NN = ws_inter.normal; // n'

    Ray directRay(p + N * eps, ws);
    Intersection directRayInter = intersect(directRay);
    //if(directRayInter.happened) puts("OK");

    if(directRayInter.happened && (x - directRayInter.position).norm() < eps){
        L_dir = ws_inter.emit.cwiseProduct(wo_inter.material->eval(-ws, -wo, N)) * N.dot(ws) * NN.dot(-ws) / ((x-p).squaredNorm() * pdf_light);
        //std :: cout << wo_inter.material->eval(-ws, -wo, N) << std :: endl;
        //puts("");
    }

    float ksi = get_random_float();

    if(ksi > RussianRoulette){
        L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
    }else{
        Eigen::Vector3f wi = wo_inter.material->sample(wo, N);
        Ray r(p, wi);
        Intersection indirectRayInter = intersect(r);
        if(!indirectRayInter.happened){
            L_indir = Eigen::Vector3f(0.0, 0.0, 0.0);
        }else{
            if(!indirectRayInter.material->hasEmission()){
                L_indir = castRay(r, depth + 1).cwiseProduct(wo_inter.material->eval(-wi, -wo, N)) * N.dot(wi) / (wo_inter.material->pdf(wo, wi, N) * RussianRoulette);
            }
        }
    }
    return wo_inter.material->getEmission() + L_dir + L_indir; // 最终的着色为材质自己的发光 + 直接光照 + 间接光照
}
