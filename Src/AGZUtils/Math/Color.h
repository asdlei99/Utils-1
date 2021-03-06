﻿#pragma once

/**
 * @file Math/Color.h
 * @brief RGB(A)颜色类型以及基本的颜色常量
 */

#include "Vec3.h"
#include "Vec4.h"

namespace AGZ::Math {

/**
 * @brief RGB颜色坐标
 */
template<typename T>
using Color3 = Vec3<T>;

/**
 * @brief RGB颜色坐标以及alpha分量
 */
template<typename T>
using Color4 = Vec4<T>;

using Color3f = Color3<float>;
using Color3d = Color3<double>;
using Color3b = Color3<uint8_t>;

using Color4f = Color4<float>;
using Color4d = Color4<double>;
using Color4b = Color4<uint8_t>;

#define COLOR_CONST(N, R, G, B, A, RB, GB, BB, AB) \
    namespace Aux \
    { \
        struct Color_##N##_t \
        { \
            constexpr Color_##N##_t() = default; \
            constexpr operator Color3f() const noexcept \
                { return Color3f(float(R),  float(G),  float(B)); } \
            constexpr operator Color3d() const noexcept \
                { return Color3d(double(R), double(G), double(B)); } \
            constexpr operator Color3b() const noexcept \
                { return Color3b(RB, GB, BB); } \
            constexpr operator Color4f() const noexcept \
                { return Color4f(float(R),  float(G), \
                                 float(B),  float(A)); } \
            constexpr operator Color4d() const noexcept \
                { return Color4d(double(R), double(G), \
                                 double(B), double(A)); } \
            constexpr operator Color4b() const noexcept \
                { return Color4b(RB, GB, BB, AB); } \
        }; \
    } \
    inline constexpr Aux::Color_##N##_t N

/**
 * @brief 常用RGB常量值
 */
namespace COLOR
{
    COLOR_CONST(VOIDC, 0.0, 0.0, 0.0, 0.0, 0,   0,   0,   0);
    COLOR_CONST(BLACK, 0.0, 0.0, 0.0, 1.0, 0,   0,   0,   255);
    COLOR_CONST(WHITE, 1.0, 1.0, 1.0, 1.0, 255, 255, 255, 255);
    COLOR_CONST(RED,   1.0, 0.0, 0.0, 1.0, 255, 0,   0,   255);
    COLOR_CONST(GREEN, 0.0, 1.0, 0.0, 1.0, 0,   255, 0,   255);
    COLOR_CONST(BLUE,  0.0, 0.0, 1.0, 1.0, 0,   0,   255, 255);
}

#undef COLOR_CONST

} // namespace AGZ::Math
