#pragma once

#include "../Misc/Common.h"
#include "Charset.h"

AGZ_NS_BEG(AGZ)

template<typename T>
class UTF32Core
{
public:

    static_assert(sizeof(T) >= 4);

    using Iterator  = const T*;
    using CodePoint = char32_t;
    using CodeUnit  = T;

    static std::string Name() { return "UTF-32"; }

    static constexpr size_t MaxCUInCP = 1;

    static size_t CUInCP(CodePoint cp)
    {
        return 1;
    }

    static size_t CP2CU(CodePoint cp, CodeUnit *cu)
    {
        cu[0] = static_cast<CodeUnit>(cp);
        return 1;
    }

    static size_t CU2CP(const CodeUnit *cu, CodePoint *cp)
    {
        AGZ_ASSERT(cu && cp);
        *cp = static_cast<CodePoint>(cu[0]);
        return 1;
    }

    static char32_t ToUnicode(CodePoint cp) { return cp; }

    static CodePoint FromUnicode(char32_t cp) { return cp; }
};

template<typename T = char32_t>
using UTF32 = Charset<UTF32Core<T>>;

AGZ_NS_END(AGZ)
