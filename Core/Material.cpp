//
// Created by yizr_cnyali on 2023/12/3.
//

#include "Material.hpp"
#include "global.hpp"
#include <cassert>

Eigen::Vector3f transform(const Eigen::Vector3f &target,
                          const Eigen::Vector3f &N) { // 用罗德里格斯旋转公式构造一个以 N 为 z 轴的标准正交基, 然后把 target 变换过去, 使得采样出射方向的半球变成以 N 为中心轴的上半球
    Eigen::Vector3f B, C;
    if (std::fabs(N.x()) > std::fabs(N.y())) {
        C = Eigen::Vector3f(N.z(), 0.0f, -N.x()).normalized();
    } else {
        C = Eigen::Vector3f(0.0f, N.z(), -N.y()).normalized();
    }
    //B = C.cross(N).normalized();
    B = C.cross(N);
    return target.x() * B + target.y() * C + target.z() * N;
}

Material::Material(MaterialType _type, Eigen::Vector3f _Emission) {
    materialType = _type;
    Emission = _Emission;
}

Material::~Material() = default;

MaterialType Material::getType() { return materialType; }

Eigen::Vector3f Material::getEmission() { return Emission; }

std::string Material::getName() { return name; }

Eigen::Vector3f
Material::Fresnel(const Eigen::Vector3f &i, const Eigen::Vector3f &N) { // Fresnel 项计算的是被反射的能量的总量, 因此 i 需要传入打到物体表面后的出射方向

    return F0 + (Eigen::Vector3f(1.0, 1.0, 1.0) - F0) * pow(1 - i.dot(N), 5);
}

float Material::Geometry1(const Eigen::Vector3f &x, const Eigen::Vector3f &h, const Eigen::Vector3f &N) {
    // Cook Torrance G GGX
    // \frac{x.dot(h)}{x.dot(N)} \times \frac{2}{1 + \sqrt(1 + a^2tan^2(\theta_x))}
    // \theta_x is the angel between x and N
    // tan^2(\theta_x) = \frac{1 - (x.dot(n)^2){(x.dot(n))^2}}
    float a = (1 - glossiness) * (1 - glossiness);
    float a2 = a * a;
    float XdotH = x.dot(h);
    float XdotN = x.dot(N);
    float tanT2 = (1 - XdotN * XdotN) / (XdotN * XdotN); // tan^2(\theta_x)
    return (XdotH / XdotN > 0 ? 1 : 0) * (2.0 / (1 + sqrt(1 + a2 * tanT2)));
}

float Material::GeometryGGX(const Eigen::Vector3f &i, const Eigen::Vector3f &o, const Eigen::Vector3f &h,
                            const Eigen::Vector3f &N) {
    return Geometry1(i, h, N) * Geometry1(o, h, N);
}

float Material::DistributionGGX(const Eigen::Vector3f &h, const Eigen::Vector3f &N) {
    // Cook Torrance D GGX
    // \frac{a^2}{\pi((N.dot(h))^2 * (a^2 - 1) + 1)^2}, a = roughness^2
    float a = (1 - glossiness) * (1 - glossiness);
    float a2 = a * a; // a^2
    float NdotH = N.dot(h);
    float m = NdotH * NdotH * (a2 - 1) + 1;
    return (NdotH > 0 ? 1 : 0) * a2 / (M_PI * m * m);
}

Eigen::Vector3f Material::sample(const Eigen::Vector3f &wi, const Eigen::Vector3f &N) {

    switch (materialType) {
        case DIFFUSE: {

            float x_1 = get_random_float(), x_2 = get_random_float();
            float z = std::fabs(1.0 - 2.0f * x_1);
            float r = std::sqrt(1.0 - z * z), phi = 2 * M_PI * x_2;

            Eigen::Vector3f localRay = Eigen::Vector3f(r * std::cos(phi), r * std::sin(phi), z);
            return transform(localRay, N).normalized();

            break;
        }
        case Mirror: {

            Eigen::Vector3f i = -wi; // wi指向的是入射点, 需要反转朝外
            //i + o = 2 * i.dot(N) * N
            return (2 * i.dot(N) * N - i).normalized(); // 对称出来就是镜面反射方向
            break;
        }
        case Glass: {
            Eigen::Vector3f i = -wi;

            Eigen::Vector3f F = i.dot(N) > 0.0f ? Fresnel(i, N) : Fresnel(i, -N);

            float P = F.x();

            if (get_random_float() < P) { // P 是由 Fresnel 项确定的发生反射的量, 用于随机采样是反射还是折射
                if (i.dot(N) > 0.0f) { // 外表面反射
                    return (2 * i.dot(N) * N - i).normalized(); // 对称出来就是镜面反射方向
                } else { // 内表面反射
                    return (2 * i.dot(-N) * (-N) - i).normalized();
                }
            } else {
                if (i.dot(N) > 0.0f) { // 外表面折射
                    float cosi = i.dot(N);
                    float etai = 1.0;
                    float etat = ior;
                    float eta = etai / etat;
                    float k = 1 - eta * eta * (1 - cosi * cosi);
                    return (k < 0.0f ? Eigen::Vector3f(0.0f, 0.0f, 0.0f) : eta * wi + (eta * cosi - std::sqrt(k)) * N).normalized();
                } else { // 内表面折射
                    float cosi = i.dot(-N);
                    float etai = ior;
                    float etat = 1.0;
                    float eta = etai / etat;
                    float k = 1 - eta * eta * (1 - cosi * cosi);
                    return (k < 0.0f ? Eigen::Vector3f(0.0f, 0.0f, 0.0f) : eta * wi + (eta * cosi - std::sqrt(k)) * (-N)).normalized();
                }
            }

            break;
        }
        case Microfacet: {
            // Microfacet Models for Refraction through Rough Surfaces https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.html

            float x_1 = get_random_float(), x_2 = get_random_float(); // xi_1, xi_2
            float a = (1 - glossiness) * (1 - glossiness); // Same as a in GGX
            float a2 = a * a;
            float theta = std::acos(std::sqrt((1 - x_1) / (x_1 * (a2 - 1) + 1)) );
            float phi = 2 * M_PI * x_2;

            Eigen::Vector3f localRay = Eigen::Vector3f( std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta));
            return transform(localRay, N).normalized();

            break;
        }
    }
}

Eigen::Vector3f Material::eval(const Eigen::Vector3f &wi, const Eigen::Vector3f &wo,
                               const Eigen::Vector3f &N) { // 真实的光照沿着 wi方向射入, 从 wo 方向射出(因此这条路径和从相机发出的"光路"是相反的)

    switch (materialType) {
        case DIFFUSE: {
            if (wo.dot(N) > 0.0f) {
                return Kd / M_PI; // TODO: 对于 Texture 来说, 实质上只是不同位置的 Kd 由贴图确定, 那么我增加一个传入交点即可进行采样
            } else {
                return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
            }
            break;
        }
        case Mirror: {
            if (wo.dot(N) > 0.0f) {
                Eigen::Vector3f i = -wi; // 这就是入射光方向
                Eigen::Vector3f F = Fresnel(i, N); // Fresnel项为反射出去的能量
                return F / N.dot(i); // 这是为了保证能量守恒, 所有出射的能量积分起来应该等于入射的能量; 从另一个角度看, 理想反射和理想折射都跟入射角度无关, 完全取决于表面的几何形状以及反射率/折射率, 因此入射角度不会导致能量损失
            } else {
                return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
            }
            break;
        }
        case Glass: {
            Eigen::Vector3f i = -wi;
            Eigen::Vector3f o = wo;
            if ((i.dot(N) > 0.0f && o.dot(N) > 0.0f) ||
                (i.dot(N) < 0.0f && o.dot(N) < 0.0f)) { // 入射光和出射光都在法线的同侧为反射, 否则为折射
                Eigen::Vector3f F = i.dot(N) > 0.0f ? Fresnel(i, N) : Fresnel(i, -N); // Fresnel项为反射出去的能量 记得讨论是外表面还是内表面!
                return F / N.dot(i);
            } else { // 理想折射 BTDF
                Eigen::Vector3f F = i.dot(N) > 0.0f ? Fresnel(i, N) : Fresnel(i, -N); // Fresnel项为反射出去的能量
                return (Eigen::Vector3f(1.0f, 1.0f, 1.0f) - F) * std::pow((wo.dot(N) > 0.0 ? 1.0 / ior : ior), 2) / N.dot(i); // 折射光与法线同向说明是从内部出射到外部
            }
            break;
        }
        case Microfacet: {
            if (wo.dot(N) > 0.0f) {

                Eigen::Vector3f i = -wi; // 为了求半程向量, 需要用入射光方向的反方向
                Eigen::Vector3f o = wo; // 这就是出射光方向
                Eigen::Vector3f h = (i + o).normalized(); // 半程向量

                Eigen::Vector3f F = Fresnel(i, N);

                float G = GeometryGGX(i, o, h, N);

                float D = DistributionGGX(h, N);

                Eigen::Vector3f specular = (F * G * D) / (4 * N.dot(i) * N.dot(o));

                //return 0.5 * (Kd / M_PI) + 0.5 * Ks;
                return (Eigen::Vector3f(1.0, 1.0, 1.0) - F).cwiseProduct(Kd / M_PI) +
                       specular; // (1 - kReflection) * diffuse + kReflection * Specular
            } else {
                return Eigen::Vector3f(0.0f, 0.0f, 0.0f);
            }
            break;
        }
    }
}

float Material::pdf(const Eigen::Vector3f &wi, const Eigen::Vector3f &wo, const Eigen::Vector3f &N) {

    switch (materialType) {
        case DIFFUSE: {
            if (wo.dot(N) > 0.0f) { // Uniform sample upper sphere, pdf = 1 / 2\pi (单位球面积为4\pi, 只有上半球面有效为 2\pi)
                return 0.5f / M_PI;
            } else {
                return 0.0f;
            }
            break;
        }
        case Mirror: { // 完美的镜面反射方向是唯一的
            if (fabs(sample(wi, N).dot(wo) - 1.0f) < eps) { // 应该和真实的采样到的方向相比
                return 1.0f;
            } else {
                return 0.0f;
            }
            break;
        }
        case Glass: {
            Eigen::Vector3f i = -wi;
            Eigen::Vector3f o = wo;

            Eigen::Vector3f F = i.dot(N) > 0.0f ? Fresnel(i, N) : Fresnel(i, -N);

            float P = F.x();

            if ((i.dot(N) > 0.0f && o.dot(N) > 0.0f) ||
                (i.dot(N) < 0.0f && o.dot(N) < 0.0f)) { // 入射光和出射光都在法线的同侧为反射, 否则为折射
                Eigen::Vector3f reflect;
                if (i.dot(N) > 0.0f) { // 外表面反射
                    reflect = 2 * i.dot(N) * N - i; // 对称出来就是镜面反射方向
                } else { // 内表面反射
                    reflect = 2 * i.dot(-N) * (-N) - i;
                }
                if(fabs(reflect.dot(wo) - 1.0f) < eps) { // 如果是反射而且是个合法的反射方向
                    return P;
                }else {
                    return 0.0f;
                }
            } else { // 折射
                Eigen::Vector3f refract;
                if (i.dot(N) > 0.0f) { // 外表面折射
                    float cosi = i.dot(N);
                    float etai = 1.0;
                    float etat = ior;
                    float eta = etai / etat;
                    float k = 1 - eta * eta * (1 - cosi * cosi);
                    refract = (k < 0.0f ? Eigen::Vector3f(0.0f, 0.0f, 0.0f) : eta * wi + (eta * cosi - std::sqrt(k)) * N).normalized();
                } else { // 内表面折射
                    float cosi = i.dot(-N);
                    float etai = ior;
                    float etat = 1.0;
                    float eta = etai / etat;
                    float k = 1 - eta * eta * (1 - cosi * cosi);
                    refract = (k < 0.0f ? Eigen::Vector3f(0.0f, 0.0f, 0.0f) : eta * wi + (eta * cosi - std::sqrt(k)) * (-N)).normalized();
                }
                if(fabs(refract.dot(wo) - 1.0f) < eps) { // 如果是折射而且是个合法的折射方向
                    return (1 - P);
                }else {
                    return 0.0f;
                }
            }
            break;
        }
        case Microfacet: {
            if (wo.dot(N) > 0.0f) {
                Eigen::Vector3f i = -wi; // 为了求半程向量, 需要用入射光方向的反方向
                Eigen::Vector3f o = wo; // 这就是出射光方向
                Eigen::Vector3f h = (i + o).normalized(); // 半程向量


                float a = (1 - glossiness) * (1 - glossiness);
                float a2 = a * a; // a^2
                float NdotH = N.dot(h);
                float m = NdotH * NdotH * (a2 - 1) + 1;
                //printf("P: %f\n", (2 * a2 * NdotH * std::sqrt(1 - NdotH * NdotH) / (m * m)) / (4 * NdotH));
                return (2 * a2 * NdotH * std::sqrt(1 - NdotH * NdotH) / (m * m)) / (4 * NdotH); // pdf = \frac{2a^2cos(\theta)sin(\theta)}{((N.dot(h))^2 * (a^2 - 1) + 1)^2} / 4NdotH, a = roughness^2
            } else {
                return 0.0f;
            }
            break;
        }
    }
}