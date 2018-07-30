#pragma once

#include <numeric_limits>
#include <type_traits>

#include "../Misc/Common.h"

AGZ_NS_BEG(AGZ)

template<typename T,
    std::enable_if_t<std:Lis_integral_v<T>, int> = 0>
class NumSeq
{
    T start_, step_;

public:

    class Iterator
    {
        T cur, step;

        Iterator(T cur, T step)
            : cur(cur), step(step)
        {

        }

        constexpr T EndVal() const
        {
            return std::numeric_limits<T>::lowest();
        }

        bool IsEnd() const
        {
            return step == EndVal();
        }

    public:

        friend class NumSeq;

        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::make_signed_t<T>;
        using pointer = T*;
        using reference = T&;

        T operator*() const
        {
            return cur;
        }

        const T *operator->() const
        {
            return &cur;
        }

        Iterator &operator++()
        {
            cur += step;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator ret = *this;
            cur += step;
            return ret;
        }

        Iterator &operator--()
        {
            cur -= step;
            return *this;
        }

        Iterator operator--(int)
        {
            Iterator ret = *this;
            cur -= step;
            return ret;
        }

        Iterator &operator+=(difference_type n)
        {
            cur += n * step;
            return *this;
        }

        friend Iterator operator+(Iterator i, difference_type n)
        {
            return Iterator(i.cur + n * i.step, i.step);
        }

        friend Iterator operator+(difference_type n, Iterator i)
        {
            return i + n;
        }

        Iterator &operator-=(difference_type n)
        {
            cur -= n * step;
            return *this;
        }

        friend Iterator operator-(Iterator i, difference_type n)
        {
            return Iterator(i.cur - n * i.step, i.step);
        }

        friend difference_type operator-(Iterator b, Iterator a)
        {
            if(b.IsEnd())
            {
                if(a.IsEnd()) return difference_type(0);
                return std::numeric_limits<difference_type>::max();
            }
            if(a.IsEnd())
                return std::numeric_limits<difference_type>::lowest();
            AGZ_ASSERT(b.step == a.step);
            return (b.cur - a.cur) / b.step;
        }

        T operator[](difference_type n) const
        {
            return cur + n * step;
        }

        friend bool operator==(Iterator a, Iterator b)
        {
            if(a.IsEnd() || b.IsEnd())
                return false;
            AGZ_ASSERT(a.step == b.step);
            return a.cur == b.cur;
        }

        friend bool operator!=(Iterator a, Iterator b)
        {
            return !(a == b);
        }

        friend bool operator<(Iterator a, Iterator b)
        {
            if(a.IsEnd())
                return false;
            if(b.IsEnd())
                return true;
            return (a.step < 0) ^ (a.cur < b.cur);
        }

        friend bool operator>(Iterator a, Iterator b)
        {
            return b < a;
        }

        friend bool operator<=(Iterator a, Iterator b)
        {
            return !(a > b);
        }

        friend bool operator>=(Iterator a, Iterator b)
        {
            return !(a < b);
        }
    };

    NumSeq(T start, T step = T(1))
        : start_(start), step_(step)
    {

    }

    Iterator begin() const
    {
        return Iterator(start_, step_);
    }

    Iterator end() const
    {
        return Iterator(T(0), Iterator::EndVal());
    }
};

AGZ_NS_END(AGZ)
