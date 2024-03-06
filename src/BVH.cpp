//
// Created by yizr_cnyali on 2023/12/18.
//

#include <cassert>
#include "BVH.hpp"

BVH::BVH(std::vector<Object *> objects) {
    puts("Create A BVH!");
    root = Build(objects);
}

BVHNode* BVH::Build(std::vector <Object*> objects){
    BVHNode* node = new BVHNode();

    Bounds3 bounds;
    for(int i = 0; i < objects.size(); i++){
        bounds = bounds.Union(objects[i]->getBounds());
    }

    if(objects.size() == 1) {
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->lson = nullptr;
        node->rson = nullptr;
        node->area = objects[0]->getArea();
        return node;
    }else if(objects.size() == 2) {
        node->lson = Build(std::vector{objects[0]});
        node->rson = Build(std::vector{objects[1]});

        node->bounds = node->lson->bounds.Union(node->rson->bounds);
        node->area = node->lson->area + node->rson->area;
        return node;
    }else {
        Bounds3 centroidBounds;
        for(int i = 0; i < objects.size(); i++){
            centroidBounds = centroidBounds.Union(objects[i]->getBounds().Centroid());
        }
        int dim = centroidBounds.maxExtent();
        if(dim == 0){
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x() < f2->getBounds().Centroid().x();
            });
        }
        if(dim == 1){
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y() < f2->getBounds().Centroid().y();
            });
        }
        if(dim == 2){
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z() < f2->getBounds().Centroid().z();
            });
        }

        auto leftObjects = std::vector<Object*>(objects.begin(), objects.begin() + objects.size() / 2);
        auto rightObjects = std::vector<Object*>(objects.begin() + objects.size() / 2, objects.end());

        assert(objects.size() == leftObjects.size() + rightObjects.size()); // 如果不相等说明划分有问题

        node->lson = Build(leftObjects);
        node->rson = Build(rightObjects);

        node->bounds = node->lson->bounds.Union(node->rson->bounds);
        node->area = node->lson->area + node->rson->area;
    }

    return node;
}

Intersection BVH::Intersect(const Ray &ray) const {

    Intersection inter;

    if(!root) return inter;

    inter = BVH::getIntersection(root, ray);

    return inter;
}

Intersection BVH::getIntersection(BVHNode* node, const Ray& ray) const
{
    // Traverse the BVH to find intersection
    if(node->object != nullptr){ // 如果已经找到了叶子, 测试与上面的物体的交点, 返回
        Intersection ret = node->object->getIntersection(ray);
        return ret;
    }

    Intersection IntersectL, IntersectR;

    if(node->lson->bounds.IntersectP(ray, ray.direction_inv)) {
        IntersectL = getIntersection(node->lson, ray); // 去左右儿子递归找交点
    }
    if(node->rson->bounds.IntersectP(ray, ray.direction_inv)){
        IntersectR = getIntersection(node->rson, ray);
    }

    return (IntersectL.distance < IntersectR.distance ? IntersectL : IntersectR); // 注意两边都有交点的时候要选择近的
}

void BVH::getSample(BVHNode *node, float p, Intersection &pos, float &pdf) {
    if(node->lson == nullptr || node->rson == nullptr){
        node->object->sample(pos, pdf); // 这个东西返回的是在 node 中采样的 pdf
        pdf *= node->area; // 因此需要乘上node的面积并最后除以整棵BVH的面积来给整体乘上采样到(选中) node 的 pdf
        return;
    }
    if(p < node->lson->area) {
        getSample(node->lson, p, pos, pdf);
    }else {
        getSample(node->rson, p - node->lson->area, pos, pdf);
    }
}

void BVH::sample(Intersection &pos, float &pdf) {
    float p = std::sqrt(get_random_float()) * root->area;
    getSample(root, p, pos, pdf);
    pdf /= root->area;
}