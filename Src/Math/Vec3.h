#pragma once

#include <type_traits>

#include "../Common.h"
#include "Scalar.h"
#include "Vec2.h"

AGZ_NS_BEG(AGZ::Math)

template<typename T>
class Vec3
{
public:

    union
    {
        struct { T u; T v; T m; };
        struct { T x; T y; T z; };
        struct { T r; T g; T b; };
    };

public:

    using Data = T[3];
    using Component = T;
    using Self = Vec3<T>;

    AGZ_FORCE_INLINE Vec3() : Vec3(T(0)) { }

    explicit AGZ_FORCE_INLINE Vec3(Uninitialized_t) { }

    explicit AGZ_FORCE_INLINE Vec3(const T &value) : x(v), y(v), z(v) { }

    AGZ_FORCE_INLINE Vec3(const T &x, const T &y, const T &z) : x(x), y(y), z(z) { }

    AGZ_FORCE_INLINE Vec3(const Self &other) : x(other.x), y(other.y), z(other.z) { }

    explicit AGZ_FORCE_INLINE Vec3(const T *data)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        std::memcpy(&x, data, sizeof(Data));
    }

    AGZ_FORCE_INLINE Self &operator=(const Self &other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    template<typename U, typename F>
    Vec3<U> Map(F &&f) const { return Vec3<U>(f(x), f(y), f(z)); }

#include "SwizzleVec3.inl"

#define x r
#define y g
#define z b

#include "SwizzleVec3.inl"

#undef x
#undef y
#undef z

#define x u
#define y v
#define z m

#include "SwizzleVec3.inl"

#undef x
#undef y
#undef z

    AGZ_FORCE_INLINE       T &operator[](size_t idx) { AGZ_ASSERT(idx < 3); return (&x)[idx]; }
    AGZ_FORCE_INLINE const T &operator[](size_t idx) const { AGZ_ASSERT(idx < 3); return (&x)[idx]; }

    AGZ_FORCE_INLINE Self operator+(const Self &rhs) const { return Self(x + rhs.x, y + rhs.y, z + rhs.z); }
    AGZ_FORCE_INLINE Self operator-(const Self &rhs) const { return Self(x - rhs.x, y - rhs.y, z - rhs.z); }
    AGZ_FORCE_INLINE Self operator*(const Self &rhs) const { return Self(x * rhs.x, y * rhs.y, z * rhs.z); }
    AGZ_FORCE_INLINE Self operator/(const Self &rhs) const { return Self(x / rhs.x, y / rhs.y, z / rhs.z); }

    AGZ_FORCE_INLINE Self &operator+=(const Self &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    AGZ_FORCE_INLINE Self &operator-=(const Self &rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    AGZ_FORCE_INLINE Self &operator*=(const Self &rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
    AGZ_FORCE_INLINE Self &operator/=(const Self &rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

    AGZ_FORCE_INLINE bool operator==(const Self &rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    AGZ_FORCE_INLINE bool operator!=(const Self &rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

    template<typename U>
    AGZ_FORCE_INLINE Self &operator+=(const U &rhs) { x += rhs; y += rhs; z += rhs; return *this; }
    template<typename U>
    AGZ_FORCE_INLINE Self &operator-=(const U &rhs) { x -= rhs; y -= rhs; z -= rhs; return *this; }
    template<typename U>
    AGZ_FORCE_INLINE Self &operator*=(const U &rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }
    template<typename U>
    AGZ_FORCE_INLINE Self &operator/=(const U &rhs) { x /= rhs; y /= rhs; z /= rhs; return *this; }

    static const Self &UNIT_X()
    {
        static const Self ret(T(1), T(0), T(0));
        return ret;
    }

    static const Self &UNIT_Y()
    {
        static const Self ret(T(0), T(1), T(0));
        return ret;
    }

    static const Self &UNIT_Z()
    {
        static const Self ret(T(0), T(0), T(1));
        return ret;
    }
};

template<typename T1, typename T2> AGZ_FORCE_INLINE
Vec3<T2> operator+(const T1 &lhs, const Vec3<T2> &rhs) { return Vec3<T2>(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z); }
template<typename T1, typename T2> AGZ_FORCE_INLINE
Vec3<T1> operator+(const Vec3<T1> &lhs, const T2 &rhs) { return Vec3<T1>(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs); }
template<typename T1, typename T2> AGZ_FORCE_INLINE
Vec3<T1> operator-(const Vec3<T1> &lhs, const T2 &rhs) { return Vec3<T1>(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs); }
template<typename T1, typename T2> AGZ_FORCE_INLINE
Vec3<T2> operator*(const T1 &lhs, const Vec3<T2> &rhs) { return Vec3<T2>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z); }
template<typename T1, typename T2> AGZ_FORCE_INLINE
Vec3<T1> operator*(const Vec3<T1> &lhs, const T2 &rhs) { return Vec3<T1>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs); }
template<typename T1, typename T2> AGZ_FORCE_INLINE
Vec3<T1> operator/(const Vec3<T1> &lhs, const T2 &rhs) { return Vec3<T1>(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs); }

template<typename T>
AGZ_FORCE_INLINE auto Cross(const Vec3<T> &lhs, const Vec3<T> &rhs)
{
    return Vec3<T>(lhs.y * rhs.z - lhs.z * rhs.y,
                   lhs.z * rhs.x - lhs.x * rhs.z,
                   lhs.x * rhs.y - lhs.y * rhs.x);
}

template<typename T1, typename T2>
AGZ_FORCE_INLINE auto Dot(const Vec3<T1> &lhs, const Vec3<T2> &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

template<typename T>
AGZ_FORCE_INLINE auto LengthSquare(const Vec3<T> &vec)
{
    return Dot(vec, vec);
}

template<typename T>
AGZ_FORCE_INLINE auto Length(const Vec3<T> &vec)
{
    return Sqrt(LengthSquare(vec));
}

template<typename T>
AGZ_FORCE_INLINE auto Normalize(const Vec3<T> &vec)
{
    return vec / Length(vec);
}

template<typename T>
AGZ_FORCE_INLINE auto Clamp(const Vec3<T> &vec, T minv, T maxv)
{
    return Vec3<decltype(Clamp(vec.x, minv, maxv))>(
                    Clamp(vec.x, minv, maxv),
                    Clamp(vec.y, minv, maxv),
                    Clamp(vec.z, minv, maxv));
}

template<typename T>
AGZ_FORCE_INLINE bool ApproxEq(const Vec3<T> &lhs, const Vec3<T> &rhs, T epsilon)
{
    return ApproxEq(lhs.x, rhs.x, epsilon) &&
           ApproxEq(lhs.y, rhs.y, epsilon) &&
           ApproxEq(lhs.z, rhs.z, epsilon);
}

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

AGZ_NS_END(AGZ::Math)
