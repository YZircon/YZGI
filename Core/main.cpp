#include "Scene.hpp"
#include "Triangle.hpp"
#include "Material.hpp"
#include "Renderer.hpp" // 渲染器类
#include "Integrator.hpp"
#include "../Integrators/PathTracing.hpp"
#include <chrono>

// TODO : Parse command (argc, argv) using getopt or CLI
// TODO : Read Scene Json File (json library)
// TODO : Config Render and Call Renderer.Render(Scene) to get the rendered picture as a matrix.
// TODO : Output rendered picture to a file
// TODO : Classes, different render path using different namespace in same file.
// TODO : 光线是要和具体的三角形求交, 我们可以维护一个数据结构以 UUID 保证相同的资源不会被重复加载, 因此三角形上只用存纹理坐标


int main() {
    Scene scene;
    Renderer r;
    PathTracingIntegrator SimplePT(10000, 0.9);


    Material* red = new Material(DIFFUSE, Eigen::Vector3f(0.0f, 0.0f, 0.0f));
    red->Kd = Eigen::Vector3f(0.63f, 0.065f, 0.05f);
    Material* green = new Material(DIFFUSE, Eigen::Vector3f(0.0f, 0.0f, 0.0f));
    green->Kd = Eigen::Vector3f(0.14f, 0.45f, 0.091f);
    Material* white = new Material(DIFFUSE, Eigen::Vector3f(0.0f, 0.0f, 0.0f));
    white->Kd = Eigen::Vector3f(0.725f, 0.71f, 0.68f);
    Material* light = new Material(DIFFUSE, (8.0f * Eigen::Vector3f(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Eigen::Vector3f(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f * Eigen::Vector3f(0.737f+0.642f,0.737f+0.159f,0.737f)));
    light->Kd = Eigen::Vector3f(0.65f, 0.65f, 0.65f);

    Material* m = new Material(Microfacet, Eigen::Vector3f(0.0, 0.0, 0.0));
    m->Kd = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
    m->Ks = Eigen::Vector3f(1.0f, 0.782f, 0.344f);
    m->glossiness = 0.7;
    m->F0 = Eigen::Vector3f(1.0f, 0.782f, 0.344f);

    Material* mirror = new Material(Mirror, Eigen::Vector3f(0.0, 0.0, 0.0));
    mirror->F0 = Eigen::Vector3f(1.0, 1.0, 1.0);

    Material* glass = new Material(Glass, Eigen::Vector3f(0.0, 0.0, 0.0));
    glass->ior = 1.5;
    glass->F0 = Eigen::Vector3f(0.04f, 0.04f, 0.04f); // 注意F0跟ior需要是匹配的, 否则可能导致反射和折射的强度分配完全不对头

    Model floor("../Core/models/cornellbox/floor.obj", white);
    Model leftsphere("../Core/models/sphere/LeftSphere.obj", mirror);
    Model rightsphere("../Core/models/sphere/RightSphere.obj", mirror);
    Model shortbox("../Core/models/cornellbox/shortbox.obj", mirror);
    Model tallbox("../Core/models/cornellbox/tallbox.obj", mirror);
    Model left("../Core/models/cornellbox/left.obj", red);
    Model right("../Core/models/cornellbox/right.obj", green);
    Model light_("../Core/models/cornellbox/light.obj", light);

    scene.add(&floor);
    scene.add(&shortbox);
    scene.add(&tallbox);
    //scene.add(&leftsphere);
    //scene.add(&rightsphere);
    scene.add(&left);
    scene.add(&right);
    scene.add(&light_);

    scene.buildBVH();

    auto start = std::chrono::system_clock::now();
    r.Render(&scene, &SimplePT);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";
    printf("Triangles Tested:%lld\n", test);
    printf("Triangles Passed the test:%lld\n", pass);
    std::cout << "Elapsed time in Intersection Test: " << duration.count() << " seconds\n";
    return 0;
}