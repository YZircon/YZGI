//
// Created by yizr_cnyali on 2023/12/18.
//
#pragma once
#ifndef YZGI_BVH_H
#define YZGI_BVH_H

#include "../Core/Bounds3.hpp"
#include "../Core/Object.hpp"
#include "../Core/Ray.hpp"
#include "../Core/Intersection.hpp"

struct BVHNode;

class BVH { // Mesh 持有三角形的包围盒构成的 BVH, Model 持有 Mesh 的包围盒构成的 BVH

public:

    BVH(std::vector <Object*> objects);

    BVHNode* Build(std::vector <Object*> objects);

    Intersection Intersect(const Ray& _ray) const;

    Intersection getIntersection(BVHNode *root, const Ray& _ray) const;

    void getSample(BVHNode* node, float p, Intersection &pos, float &pdf);
    void sample(Intersection &pos, float &pdf);

private:
    BVHNode *root;
};

struct BVHNode {

    Bounds3 bounds;

    BVHNode *lson;
    BVHNode *rson;

    Object* object;

    float area; // 当前结点内所有 Triangle 的面积和, 用于采样

public:
    BVHNode(){
        bounds = Bounds3();

        lson = nullptr;
        rson = nullptr;
        object = nullptr;
    }
};

#endif //YZGI_BVH_H
