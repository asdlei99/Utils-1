#pragma once

#include <cstdint>
#include <random>

#include "../Common.h"

AGZ_NS_BEG(AGZ)
AGZ_NS_BEG(Math)
AGZ_NS_BEG(Random)

template<typename T, typename S> struct Uniform_t;

struct SharedRandomEngine_t
{
    static auto &GetEng()
    {
        static thread_local std::default_random_engine eng;
        return eng;
    }
};

template<typename T, typename S> struct IntUniform_t
{
    AGZ_FORCE_INLINE T operator()(T minv, T maxv)
    {
        return std::uniform_int_distribution<T>(minv, maxv)(S::GetEng());
    }
};

#define MAKE_INT_UNIFORM_T(T) \
    template<typename S> struct Uniform_t<T, S> : IntUniform_t<T, S> { }

MAKE_INT_UNIFORM_T(std::int8_t);
MAKE_INT_UNIFORM_T(std::int16_t);
MAKE_INT_UNIFORM_T(std::int32_t);
MAKE_INT_UNIFORM_T(std::int64_t);

MAKE_INT_UNIFORM_T(std::uint8_t);
MAKE_INT_UNIFORM_T(std::uint16_t);
MAKE_INT_UNIFORM_T(std::uint32_t);
MAKE_INT_UNIFORM_T(std::uint64_t);

#undef MAKE_INT_UNIFORM_T

template<typename T, typename S> struct RealUniform_t
{
    AGZ_FORCE_INLINE T operator()(T minv, T maxv)
    {
        return std::uniform_real_distribution<T>(minv, maxv)(S::GetEng());
    }
};

#define MAKE_REAL_UNIFORM_T(T) \
    template<typename S> struct Uniform_t<T, S> : RealUniform_t<T, S> { }

MAKE_REAL_UNIFORM_T(float);
MAKE_REAL_UNIFORM_T(double);

#undef MAKE_REAL_UNIFORM_T

template<typename T, typename S = SharedRandomEngine_t>
inline Uniform_t<T, S> Uniform;

AGZ_NS_END(Random)
AGZ_NS_END(Math)
AGZ_NS_END(AGZ)
