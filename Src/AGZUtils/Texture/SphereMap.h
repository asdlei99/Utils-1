﻿#pragma once

#include "../Utils/Math.h"

namespace AGZ {

/**
 * @brief 球面环境光的纹理坐标映射
 * 
 * 把一个理想镜面反射的球体放在场景中间，然后用正交投影摄像机去看它，看到的球面就可以反映环境光。
 * 仔细一想，没错，这居然可以表示完整的environment lighting，很棒的样子。
 * 
 * 不过话说回来，直接把(theta, phi)归一化之后编码成uv有什么不好，搞不懂这个SphereMap有什么意义。
 */
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
class SphereMapper
{
public:

    /**
     * 求能给出沿着-dir方向的环境光的texel对应的uv坐标，(0, 0)对应左上角
     * 
     * 假设是沿着-x方向看的球体，球体充斥了整个画面，和图像的四个边缘都正好相切
     */
    static Math::Vec2<T> Map(const Math::Vec3<T> &dir);

    static Math::Vec3<T> InvMap(const Math::Vec2<T> &uv);
};

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> N>
Math::Vec2<T> SphereMapper<T, N>::Map(const Math::Vec3<T> &dir)
{
    auto nor = (dir.Normalize() + Math::Vec3<T>::UNIT_X()).Normalize();
    if(Math::FP<T>(nor.y).ApproxEq(T(0)) && Math::FP<T>(nor.z).ApproxEq(T(0)))
        return { T(1), T(0.5) };
    T u = Math::Clamp<T>(T(0.5) + T(0.5) * nor.y, T(0), T(1));
    T v = Math::Clamp<T>(T(0.5) - T(0.5) * nor.z, T(0), T(1));
    return { u, v };
}

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> N>
Math::Vec3<T> SphereMapper<T, N>::InvMap(const Math::Vec2<T> &uv)
{
    auto ny = T(2) * uv.u - T(1);
    auto nz = T(1) - T(2) * uv.v;
    auto nx = Math::Sqrt(Math::Max(T(0), T(1) - ny * ny - nz * nz));
    Math::Vec3<T> nor = Math::Vec3<T>(nx, ny, nz).Normalize();
    return T(2) * Dot(nor, Math::Vec3<T>::UNIT_X()) * nor - Math::Vec3<T>::UNIT_X();
}

} // namespace AGZ
