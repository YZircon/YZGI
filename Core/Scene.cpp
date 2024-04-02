//
// Created by yizr_cnyali on 2023/12/3.
//

#include "Scene.hpp"
#include "global.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

void Scene::buildBVH(){
    this->bvh = new BVH(objects);
}


#include <chrono>

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