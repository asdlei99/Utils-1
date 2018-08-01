#pragma once

#include "../Misc/Common.h"
#include "Iterator.h"
#include "Transform.h"

AGZ_NS_BEG(AGZ)

namespace RangeAux
{
    template<typename R, typename I, typename F>
    class PartialFoldlImpl
    {
        R range_;
        I init_;
        mutable F func_;

    public:

        class Iterator
        {
            using It = typename R::Iterator;

            It it;
            I lastVal;
            mutable F *f;

        public:

            using iterator_category = std::forward_iterator_tag;
            using value_type        = I;
            using difference_type   = typename It::difference_type;
            using pointer           = ValuePointer<I>;
            using reference         = I&;

            Iterator(It it, I lastVal, F *f)
                : it(std::move(it)), lastVal(std::move(lastVal)), f(f)
            {

            }

            value_type operator*() const
            {
                return f(lastVal, *it);
            }

            pointer operator->() const
            {
                return pointer(**this);
            }

            Iterator &operator++()
            {
                lastVal = **this;
                ++it;
                return *this;
            }

            Iterator operator++(int)
            {
                auto ret = *this;
                ++*this;
                return ret;
            }

            bool operator==(const Iterator &rhs) const
            {
                AGZ_ASSERT(f == rhs.f);
                return it == rhs.it;
            }

            bool operator!=(const Iterator &rhs) const
            {
                return !(*this == rhs);
            }
        };

        PartialFoldlImpl(R range, I init, F f)
            : range_(std::move(range)),
              init_(std::move(init)),
              func_(std::move(f))
        {

        }

        Iterator begin() const
        {
            return Iterator(std::begin(range_), init_, &func_);
        }

        Iterator end() const
        {
            return Iterator(std::end(range_), init_, &func_);
        }
    };

    struct PartialFoldlTrait
    {
        template<typename R>
        using Impl = PartialFoldlImpl<R>;
    };
}

template<typename I, typename F>
auto PartialFoldl(I &&init, F &&func)
{
    return RangeAux::TransformWrapper<
            RangeAux::PartialFoldlTrait,
            remove_rcv_t<I>,
            remove_rcv_t<F>>(
                std::forward<I>(init),
                std::forward<F>(func));
}

AGZ_NS_END(AGZ)
