﻿#pragma once

#include <atomic>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../Misc/Common.h"

#include "UTF8.h"
#include "UTF32.h"

AGZ_NS_BEG(AGZ)

template<typename CS, typename TP> class String;

// Possible encoding used by a c-style string
enum class CharEncoding { UTF8, UTF32 };

namespace StringAux
{
    template<typename T>
    void CopyConstruct(T *dst, const T *src, size_t n)
    {
        for(size_t i = 0; i < n; ++i)
            new(dst++) T(*src++);
    }

    struct MultiThreaded
    {
        using RefCounter = std::atomic<size_t>;
        static constexpr bool IsThreadSafe = true;
    };

    struct SingleThreaded
    {
        using RefCounter = size_t;
        static constexpr bool IsThreadSafe = false;
    };

    // Reference counted buffer for large string storage
    template<typename CU, typename TP>
    class RefCountedBuf
    {
        mutable typename TP::RefCounter refs;
        CU data[1];

    public:

        using RefCounter = typename TP::RefCounter;
        using Self = RefCountedBuf<CU, TP>;

        static constexpr bool IsThreadSafe = TP::IsThreadSafe;

        RefCountedBuf()               = delete;
        RefCountedBuf(const Self &)   = delete;
        ~RefCountedBuf()              = delete;

        Self &operator=(const Self &) = delete;

        void IncRef() const { ++refs; }
        void DecRef() { if(!--refs) std::free(this); }

        CU *GetData() { return &data[0]; }

        static Self *New(size_t n)
        {
            AGZ_ASSERT(n >= 1);
            size_t bytes = sizeof(Self) + (n-1) * sizeof(CU);
            Self *ret = reinterpret_cast<Self*>(std::malloc(bytes));
            ret->refs = 1;
            return ret;
        }
    };

    // Valid location in string
    // Can never locate at the medium of any code point
    template<typename CU>
    struct Loc
    {
        const CU *pos;

        Loc() : pos(nullptr) { }
        Loc(const CU *pos) : pos(pos) { }
        bool IsNull() const { return pos == nullptr; }
    };
}

class EncodingException : public std::invalid_argument
{
public:
    EncodingException(const std::string &err) : invalid_argument(err) { }
};

// CU: Code Unit
// CP: Code Point
// TP: Thread Policy
template<typename CS = UTF8<char>, typename TP = StringAux::MultiThreaded>
class String
{
    static constexpr size_t SMALL_BUF_SIZE = 31;

    using LargeBuf = StringAux::RefCountedBuf<typename CS::CodeUnit, TP>;

    union
    {
        struct
        {
            typename CS::CodeUnit buf[SMALL_BUF_SIZE];
            std::uint8_t len;
        } small_;

        struct
        {
            LargeBuf *buf;
            const typename CS::CodeUnit *beg;
            const typename CS::CodeUnit *end;
        } large_;
    };

    bool IsSmallStorage() const;
    bool IsLargeStorage() const;

    size_t GetSmallLen() const;
    size_t GetLargeLen() const;
    size_t GetLen() const;

    void Init(const typename CS::CodeUnit *beg,
              const typename CS::CodeUnit *end);
    void Init2(const typename CS::CodeUnit *beg1,
               const typename CS::CodeUnit *end1,
               const typename CS::CodeUnit *beg2,
               const typename CS::CodeUnit *end2);

    Self &CopyFromSelf(const Self &copyFrom);

public:

    using CharSet   = CS;
    using CodeUnit  = typename CS::CodeUnit;
    using CodePoint = typename CS::CodePoint;
    using Self      = String<CS, TP>;

    using Loc = StringAux::Loc<CodeUnit>;

    String();

    // Construct from existed buffer
    // Will copy all data to owned storage
    // Faster than the checking version bellow. UB when [beg, end) is invalid
    String(CONS_FLAG_NOCHECK_t, const CodeUnit *beg, const CodeUnit *end);

    // Construct from existed buffer
    // Will copy all data to owned storage
    String(const CodeUnit *beg, const CodeUnit *end);

    // Construct with concat([beg1, end1), [beg2, end2))
    String(CONS_FLAG_NOCHECK_t, const CodeUnit *beg1, const CodeUnit *end1,
                                const CodeUnit *beg2, const CodeUnit *end2);

    // Construct with concat([beg1, end1), [beg2, end2))
    String(const CodeUnit *beg1, const CodeUnit *end1,
           const CodeUnit *beg2, const CodeUnit *end2);

    // Construct from existed buffer
    // Will copy all data to owned storage
    String(const CodeUnit *beg, size_t n) : String(beg, beg + n) { }

    // Construct from existed buffer using another CharSet
    template<typename OCS>
    String(CONS_FLAG_FROM_t<OCS>, const typename OCS::CodeUnit *beg,
                                  const typename OCS::CodeUnit *end);

    // Construct from existed buffer using another CharSet
    template<typename OCS>
    String(CONS_FLAG_FROM_t<OCS>, const typename OCS::CodeUnit *beg,
                                  size_t n);

    template<typename OCS, typename OTP>
    String(const String<OCS, OTP> &copyFrom);

    String(Self &&moveFrom);

    String(const CodeUnit *beg, const CodeUnit *end, size_t repeat);

    String(const char *cStr,
           CharEncoding encoding = CharEncoding::UTF8);

    String(const std::string &cppStr,
           CharEncoding encoding = CharEncoding::UTF8);

    template<typename OCS, typeame OTP>
    Self &operator=(const String<OCS, OTP> &copyFrom);

    Self &operator=(Self &&moveFrom);

    ~String();

    void &Swap(Self &other);

    // Set with a new value and return the old one
    Self &Exchange(const Self &value);

    const CodeUnit *Data() const;

    size_t Length() const;

    bool IsEmpty() const;

    CodeUnit operator[](size_t idx) const;

    std::string ToStdString() const;

    Self operator+(const Self &rhs);

    Self operator*(size_t n);

    Loc FindSubstr(const Self &dst) const;
};

template<typename CS, typename TP>
String<CS, TP> operator*(size_t n, const String<CS, TP> &s);

template<typename CS, typaname TP>
std::ostream &operator<<(std::ostream &out, const String<CS, TP> &s);

using Str8  = String<UTF8<>>;
using Str32 = String<UTF32<>>;

AGZ_NS_END(AGZ)

#include "String.inl"
