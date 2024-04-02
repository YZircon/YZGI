//
// Created by yizr_cnyali on 2023/12/4.
//
#pragma once

#ifndef YZGI_TRIANGLE_HPP
#define YZGI_TRIANGLE_HPP

// 在 OBJ_Loader.h 里手动 undef 掉了 OBJL_CONSOLE_OUTPUT , 使得 OBJ Loader 不会往 console 输出东西

#include "../DS/BVH.hpp"
#include "Intersection.hpp"
#include "Material.hpp"
#include "OBJ_Loader.h"
#include "Object.hpp"
#include "Triangle.hpp"
#include "Bounds3.hpp"

#include <map>

struct Vertex {
    Eigen::Vector3f pos;
    Eigen::Vector3f normal;
    Eigen::Vector2f texCoords;
};

class Triangle : public Object {
public:
    Triangle(Vertex _v0, Vertex _v1, Vertex _v2, Material *_m = nullptr) {

        v0 = _v0.pos, v1 = _v1.pos, v2 = _v2.pos;
        n0 = _v0.normal, n1 = _v1.normal, n2 = _v2.normal;
        e1 = (v1 - v0), e2 = (v2 - v0);
        area = e1.cross(e2).norm() * 0.5; // 叉积可以认为是大平行四边形的面积
        normal = e1.cross(e2).normalized();

        t0 = _v0.texCoords, t1 = _v1.texCoords, t2 = _v2.texCoords;

        m = _m;
    }

    Intersection getIntersection(Ray ray) override;

    bool hasEmit() override {

        return m->hasEmission();
    }

    bool intersect(Ray ray) override {

        return getIntersection(ray).happened;
    }

    float getArea() override {

        return area;
    }

    Bounds3 getBounds() override;

    void sample(Intersection &inter, float &pdf) override {

        float x = std::sqrt(get_random_float()), y = get_random_float();
        inter.position = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
        inter.normal = this->normal;
        pdf = 1 / area;
    }


private:
    Eigen::Vector3f v0, v1, v2; // 三个点
    Eigen::Vector3f n0, n1, n2; // 三个点的法向量(如果有的话)
    Eigen::Vector3f e1, e2; // (v0->v1) , (v0->v2)
    Eigen::Vector2f t0, t1, t2; // 三个顶点的纹理坐标
    Eigen::Vector3f normal;

    float area;

    Material *m = nullptr; // 指向这个三角形的材质的指针, 材质实际上会由 Mesh 管理
};

class Mesh : public Object { // 一个独立的三角形组
public:
    Mesh(std::vector<Object *> *_triangles, Material *_m);

    bool intersect(Ray ray) override {

        return getIntersection(ray).happened;
    }

    bool hasEmit() override {

        return m->hasEmission();
    }

    void sample(Intersection &inter, float &pdf) override {
        /*float p = get_random_float() * area;
        float sum = 0.0;
        for(uint32_t k = 0; k < triangles->size(); k++){
            sum += (*triangles)[k]->getArea();
            if(sum >= p){
                (*triangles)[k]->sample(inter, pdf);
                break;
            }
        }*/
        bvh->sample(inter, pdf);
        inter.emit = m->getEmission();
    }

    Bounds3 getBounds() { return bounding_box; }

    float getArea() override {

        return area;
    }

    Intersection getIntersection(Ray ray) override;

private:
    Bounds3 bounding_box;
    BVH *bvh;

    std::vector<Object *> *triangles;
    Material *m;

    float area;
};

class Model : public Object { // 由一个或多个三角形组构成的模型
public:
    Model(std::string path, Material *_m);

    Intersection getIntersection(Ray ray) override;

    bool hasEmit() override {

        for (auto it: meshes) {
            if (it->hasEmit()) {
                return true;
            }
        }
        return false;
    }

    bool intersect(Ray ray) override {

        return getIntersection(ray).happened;
    }

    float getArea() override {

        return area;
    }

    Bounds3 getBounds() { return bounding_box; }

    void sample(Intersection &inter, float &pdf) override {
        /*float p = get_random_float() * area;
        float sum = 0.0;
        for(uint32_t k = 0; k < meshes.size(); k++){
            sum += meshes[k]->getArea();
            if(sum >= p){
                meshes[k]->sample(inter, pdf);
                break;
            }
        }
        pdf = 1 / area;*/
        bvh->sample(inter, pdf);
    }

private:
    Bounds3 bounding_box;
    BVH *bvh;

    std::vector<Object *> meshes;
    std::map<std::string, Material *> materials;

    float area;
};

Mesh::Mesh(std::vector<Object *> *_triangles, Material *_m) {

    triangles = _triangles;
    bvh = new BVH(*triangles); // 因为mesh用的是一个放三角形指针的数组的指针, 所以要记得解引用来建BVH
    m = _m;
    area = 0;
    for (auto it: *triangles) {
        area += it->getArea();
        bounding_box = bounding_box.Union(it->getBounds());
    }
}


Model::Model(std::string path, Material *_m = nullptr) {

    objl::Loader loader;

    area = 0;
    if (loader.LoadFile(path)) { // 换行是这玩意弄出来的? 671行那个ifdef好像不知道为啥过去了
        if (_m == nullptr) {
            for (int i = 0; i < loader.LoadedMaterials.size(); i++) { // TODO: 根据obj的信息加载材质
                Material *mat = new Material(DIFFUSE, Eigen::Vector3f(0, 0, 0));
                materials[mat->getName()] = mat; // 目前如果用的是手动实例化的 Material, 由于没有名字, 不会被存储到 materials 中 TODO: 添加名字
            }
        }
        for (int i = 0; i < loader.LoadedMeshes.size(); i++) {
            std::vector<Object *> *meshTri = new std::vector<Object *>;
            Material *meshMat;

            if (_m == nullptr) {
                if (materials.find(loader.LoadedMeshes[i].MeshMaterial.name) != materials.end()) {
                    meshMat = materials[loader.LoadedMeshes[i].MeshMaterial.name];
                } else {
                    meshMat = new Material(DIFFUSE,
                                           Eigen::Vector3f(0, 0, 0)); // 这个地方应该是个错误材质, 就是既没有指定材质模型又没有附带材质的时候显示的东西
                }
            } else {
                meshMat = _m; // 如果指定了Material, 则使用指定的Material 注意一下, 此处指定material由于它没有名字所以暂时没有被存入map中
            }

            for (int j = 0; j < loader.LoadedMeshes[i].Indices.size(); j += 3) {
                auto _v0 = loader.LoadedMeshes[i].Vertices[loader.LoadedMeshes[i].Indices[j]],
                        _v1 = loader.LoadedMeshes[i].Vertices[loader.LoadedMeshes[i].Indices[j + 1]],
                        _v2 = loader.LoadedMeshes[i].Vertices[loader.LoadedMeshes[i].Indices[j + 2]];

                Vertex v0 = {
                        .pos = Eigen::Vector3f(_v0.Position.X, _v0.Position.Y, _v0.Position.Z),
                        .normal = Eigen::Vector3f(_v0.Normal.X, _v0.Normal.Y, _v0.Normal.Z),
                        .texCoords = Eigen::Vector2f(_v0.TextureCoordinate.X, _v0.TextureCoordinate.Y)};
                Vertex v1 = {
                        .pos = Eigen::Vector3f(_v1.Position.X, _v1.Position.Y, _v1.Position.Z),
                        .normal = Eigen::Vector3f(_v1.Normal.X, _v1.Normal.Y, _v1.Normal.Z),
                        .texCoords = Eigen::Vector2f(_v1.TextureCoordinate.X, _v1.TextureCoordinate.Y)};
                Vertex v2 = {
                        .pos = Eigen::Vector3f(_v2.Position.X, _v2.Position.Y, _v2.Position.Z),
                        .normal = Eigen::Vector3f(_v2.Normal.X, _v2.Normal.Y, _v2.Normal.Z),
                        .texCoords = Eigen::Vector2f(_v2.TextureCoordinate.X, _v2.TextureCoordinate.Y)};

                Triangle *tri = new Triangle(v0, v1, v2,
                                             meshMat); // 一个潜在的问题是如果不同的Mesh有共用三角形且材质相同会导致实例化同一个三角形多次(但扬掉也不合适吧? 扬掉就不是不同mesh了)
                meshTri->emplace_back(tri);
            }
            Mesh *mesh = new Mesh(meshTri, meshMat);

            meshes.emplace_back(mesh);
            area += mesh->getArea();
            bounding_box = bounding_box.Union(mesh->getBounds());

        }
        bvh = new BVH(meshes);
    } else {
        std::cerr << "Failed to load model at " << path << std::endl;
    }
}

std::chrono::duration<double> duration;
inline Intersection Model::getIntersection(Ray ray) {

    Intersection inter;

    /*
    inter.distance = std::numeric_limits<float>::max();
    for(const auto mesh : meshes){
        Intersection now = mesh->getIntersection(ray);
        if(now.happened && now.distance < inter.distance){
            inter = now;
        }
    }*/
    inter = bvh->Intersect(ray);

    return inter;
}

inline Intersection Mesh::getIntersection(Ray ray) { // Mesh 和 ray 的最近交点
    auto start = std::chrono::steady_clock::now();
    Intersection inter;
    /*inter.distance = std::numeric_limits<float>::max();
    for(const auto triangle : *triangles){
        Intersection now = triangle->getIntersection(ray);
        if(now.happened && now.distance < inter.distance){
            inter = now;
        }
    }*/
    inter = bvh->Intersect(ray);

    auto end = std::chrono::steady_clock::now();
    duration += std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    return inter;
}

inline Bounds3 Triangle::getBounds() { return Bounds3(v0, v1).Union(v2); }

int test = 0, pass = 0;

inline Intersection Triangle::getIntersection(Ray ray) {
    test++;
    Intersection inter; // 这些都是 Möller–Trumbore 相交算法前置计算的信息

    if ((ray.direction).dot(normal) > 0)
        return inter;
    double u, v, t_tmp = 0;

    Eigen::Vector3f pvec = (ray.direction).cross(e2);
    double det = e1.dot(pvec);
    if (fabs(det) < eps)
        return inter;

    double det_inv = 1. / det;
    Eigen::Vector3f tvec = ray.origin - v0;
    u = tvec.dot(pvec) * det_inv;
    if (u < 0 || u > 1)
        return inter;
    Eigen::Vector3f qvec = tvec.cross(e1);
    v = (ray.direction).dot(qvec) * det_inv;
    if (v < 0 || u + v > 1)
        return inter;
    t_tmp = e2.dot(qvec) * det_inv;

    if (t_tmp < 0) return inter; // 这里一定要注意!! 小于0说明交点在起点与光线的反方向, 那就是没有交点!! 以前没发现是因为包围盒的IntersectP排除了大量错误的交点

    // TODO find ray triangle intersection
    inter.happened = true;// 用面积判target是否在三角形内
    inter.position = v0 + u * e1 + v * e2;// 交点坐标
    inter.distance = t_tmp;// 传输到交点的距离(这里用的时间而不是空间距离)
    inter.normal = normal;// 目前填的是交点处(所在三角形)的法向量
    inter.object = this; // 目前的理解是相交发生的物体是当前的三角形
    inter.material = m; //　目前用的是它所属的三角形的material
    //inter这几个不太确定的参数要填啥w*/
    if(inter.happened) pass++;
    return inter;
}

#endif //YZGI_TRIANGLE_HPP