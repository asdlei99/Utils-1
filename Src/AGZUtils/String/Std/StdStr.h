﻿#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "../../Misc/Common.h"
#include "../../Misc/TypeOpr.h"
#include "UTF.h"

namespace AGZ::Str
{

/**
 * @brief 判断一个字符是否是空白字符
 */
template<typename TChar, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
bool IsWhitespace(TChar ch) noexcept
{
    return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\v' || ch == '\f';
}

/**
 * @brief 判断一个字符是否是小写英文字母
 */
template<typename TChar, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
bool IsLower(TChar ch) noexcept
{
    return 'a' <= ch && ch <= 'z';
}

/**
 * @brief 判断一个字符是否是大写英文字母
 */
template<typename TChar, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
bool IsUpper(TChar ch) noexcept
{
    return 'A' <= ch && ch <= 'Z';
}

/**
 * @brief 判断一个字符是否是英文字母
 */
template<typename TChar, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
bool IsAlpha(TChar ch) noexcept
{
    return IsLower(ch) || IsUpper(ch);
}

/**
 * @brief 判断一个字符是否是十进制数字
 */
template<typename TChar, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
bool IsDemDigit(TChar ch) noexcept
{
    return '0' <= ch && ch <= '9';
}

/**
 * @brief 将小写字母转换为大写字母；若输入不是小写字母，则原样返回
 */
template<typename TChar, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
TChar ToUpper(TChar ch) noexcept
{
    return IsLower(ch) ? (ch - 'a' + 'A') : ch;
}

/**
 * @brief 将大写字母转换为小写字母；若输入不是大写字母，则原样返回
 */
template<typename TChar, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
TChar ToLower(TChar ch) noexcept
{
    return IsUpper(ch) ? (ch - 'A' + 'a') : ch;
}

namespace Impl
{
    template<typename T>               struct CanConvertToStringViewImpl                                : std::false_type {};
    template<typename TChar>           struct CanConvertToStringViewImpl<std::basic_string<TChar>>      : std::true_type { using Char = TChar; };
    template<typename TChar>           struct CanConvertToStringViewImpl<std::basic_string_view<TChar>> : std::true_type { using Char = TChar; };
    template<typename TChar>           struct CanConvertToStringViewImpl<TChar*>                        : std::true_type { using Char = TChar; };
    template<typename TChar, size_t N> struct CanConvertToStringViewImpl<TChar[N]>                      : std::true_type { using Char = TChar; };

    template<typename T> constexpr bool CanConvertToStringView_v = CanConvertToStringViewImpl<T>::value;
    template<typename T> using ConvertToStringViewCharType_t     = typename CanConvertToStringViewImpl<T>::Char;
}

#define CONV_T(TYPE)     std::enable_if_t<Impl::CanConvertToStringView_v<TYPE>,int> =0
#define CONV(VAL)        std::basic_string_view<Impl::ConvertToStringViewCharType_t<remove_rcv_t<decltype(_##VAL)>>> VAL(_##VAL)
#define TCHAR(TYPE)      Impl::ConvertToStringViewCharType_t<remove_rcv_t<TYPE>>
#define TCHAR_EQ(T1, T2) std::enable_if_t<std::is_same_v<TCHAR(T1), TCHAR(T2)>, int> =0

/**
 * @brief 将字符串中的小写英文字母原地转换为大写形式
 */
template<typename TChar>
std::basic_string<TChar> &ToUpperInPlace(std::basic_string<TChar> &str) noexcept
{
    for(size_t i = 0; i < str.size(); ++i)
        str[i] = ToUpper(str[i]);
    return str;
}

/**
 * @brief 给定一个字符串，返回将其中的所有小写英文字母转换为大写形式后的结果
 */
template<typename T, CONV_T(T)>
std::basic_string<TCHAR(T)> ToUpper(const T &_str)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return ToUpperInPlace(ret);
}

/**
 * @brief 将字符串中的大写英文字母原地转换为小写形式
 */
template<typename TChar>
std::basic_string<TChar> &ToLowerInPlace(std::basic_string<TChar> &str) noexcept
{
    for(size_t i = 0; i < str.size(); ++i)
        str[i] = ToLower(str[i]);
    return str;
}

/**
 * @brief 给定一个字符串，返回将其中的所有大写英文字母转换为小写形式后的结果
 */
template<typename T, CONV_T(T)>
std::basic_string<TCHAR(T)> ToLower(const T &_str)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return ToLowerInPlace(ret);
}

/**
 * @brief 原地删除字符串开头所有满足指定谓词的字符
 */
template<typename TChar, typename TPred>
std::basic_string<TChar> &TrimLeftInPlace(std::basic_string<TChar> &str, TPred &&pred)
{
    if(str.empty())
        return str;

    auto it = str.begin();
    while(it != str.end())
    {
        if(!pred(*it))
            break;
        ++it;
    }

    str.erase(str.begin(), it);
    return str;
}

/**
 * @brief 给定一个字符串，返回删除它开头所有满足给指定谓词的字符后的结果
 */
template<typename T, typename TPred, CONV_T(T)>
std::basic_string<TCHAR(T)> TrimLeft(const T &_str, TPred &&pred)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return TrimLeftInPlace(ret, std::forward<TPred>(pred));
}

/**
 * @brief 原地删除字符串开头所有的空白字符
 */
template<typename TChar>
std::basic_string<TChar> &TrimLeftInPlace(std::basic_string<TChar> &str)
{
    return TrimLeftInPlace(str, IsWhitespace<TChar>);
}

/**
 * @brief 给定一个字符串，返回删除它开头所有空白字符后的结果
 */
template<typename T, CONV_T(T)>
std::basic_string<TCHAR(T)> TrimLeft(const T &_str)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return TrimLeftInPlace(ret);
}

/**
 * @brief 原地删除字符串末尾所有满足给定谓词的字符
 */
template<typename TChar, typename TPred>
std::basic_string<TChar> &TrimRightInPlace(std::basic_string<TChar> &str, TPred &&pred)
{
    if(str.empty())
        return str;

    auto it = str.end();
    do
    {
        if(!pred(*(it - 1)))
            break;
        --it;
    } while(it != str.begin());

    str.erase(it, str.end());
    return str;
}

/**
 * @brief 给定一个字符串，返回删除它末尾所有满足指定谓词的字符后的结果
 */
template<typename T, typename TPred, CONV_T(T)>
std::basic_string<TCHAR(T)> TrimRight(const T &_str, TPred &&pred)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return TrimRightInPlace(ret, std::forward<TPred>(pred));
}

/**
 * @brief 原地删除字符串末尾所有空白字符
 */
template<typename TChar>
std::basic_string<TChar> &TrimRightInPlace(std::basic_string<TChar> &str)
{
    return TrimRightInPlace(str, IsWhitespace<TChar>);
}

/**
 * @brief 给定一个字符串，返回删除它末尾所有空白字符后的结果
 */
template<typename T, CONV_T(T)>
std::basic_string<TCHAR(T)> TrimRight(const T &_str)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return TrimRightInPlace(ret);
}

/**
 * @brief 原地删除字符串首尾所有满足给定谓词的字符
 */
template<typename TChar, typename TPred>
std::basic_string<TChar> &TrimInPlace(std::basic_string<TChar> &str, TPred &&pred)
{
    return TrimRightInPlace(TrimLeftInPlace(str, pred), pred);
}

/**
 * @brief 给定一个字符串，返回删除它首尾所有满足指定谓词的字符后的结果
 */
template<typename T, typename TPred, CONV_T(T)>
std::basic_string<TCHAR(T)> Trim(const T &_str, TPred &&pred)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return TrimInPlace(ret, std::forward<TPred>(pred));
}

/**
 * @brief 原地删除字符串首尾所有空白字符
 */
template<typename TChar>
std::basic_string<TChar> &TrimInPlace(std::basic_string<TChar> &str)
{
    return TrimInPlace(str, IsWhitespace<TChar>);
}

/**
 * @brief 给定一个字符串，返回删除它首尾所有空白字符后的结果
 */
template<typename T, CONV_T(T)>
std::basic_string<TCHAR(T)> Trim(const T &_str)
{
    CONV(str);
    std::basic_string<TCHAR(T)> ret(str);
    return TrimInPlace(ret);
}

/**
 * @brief 用一个字符连接一组字符串
 * @param joiner 用来连接的字符
 * @param begin 被连接的字符串容器的起始迭代器
 * @param end 被连接的字符串容器的终止迭代器
 */
template<typename TChar, typename TIterator, std::enable_if_t<std::is_integral_v<TChar>, int> = 0>
std::basic_string<TChar> Join(TChar joiner, TIterator begin, TIterator end)
{
    if (begin == end)
        return std::basic_string<TChar>();

    std::basic_string<TChar> ret = *begin++;
    while (begin != end)
    {
        ret.push_back(joiner);
        ret.append(*begin++);
    }

    return ret;
}

/**
 * @brief 用一个字符串连接一组字符串
 * @param _joiner 用来连接的字符串
 * @param begin 被连接的字符串容器的起始迭代器
 * @param end 被连接的字符串容器的终止迭代器
 */
template<typename T, typename TIterator, CONV_T(T)>
std::basic_string<TCHAR(T)> Join(const T &_joiner, TIterator begin, TIterator end)
{
    CONV(joiner);

    if(begin == end)
        return std::basic_string<TCHAR(T)>();

    std::basic_string<TCHAR(T)> ret = *begin++;
    while(begin != end)
    {
        ret.append(joiner);
        ret.append(*begin++);
    }

    return ret;
}

/**
 * @brief 用给定的字符谓词分割字符串
 * @param _src 待分割的字符串，瑟瑟发抖中
 * @param pred 字符谓词，凡满足该谓词的字符均被视为分割字符
 * @param outIterator 输出迭代器，接收 std::basic_string_view<TChar> 类型的结果
 * @param removeEmptyResult 是否移除分割结果中的空字符串，缺省为 true
 * @return 分割后得到了多少个子串
 */
template<typename T, typename TPred, typename TOutIterator, CONV_T(T)>
size_t Split(
    const T &_src, TPred &&pred,
    TOutIterator outIterator, bool removeEmptyResult = true)
{
    CONV(src);

    size_t beg = 0, ret = 0;
    while(beg < src.size())
    {
        size_t end = beg;
        while(end < src.size() && !pred(src[end]))
            ++end;
        if(end != beg || !removeEmptyResult)
        {
            ++ret;
            outIterator = src.substr(beg, end - beg);
            ++outIterator;
        }
        beg = end + 1;
    }
    return ret;
}

/**
 * @brief 用给定的字符串分割字符串
 * @param _src 待分割的字符串，瑟瑟发抖中
 * @param _splitter 用于作为分割串的字符串
 * @param outIterator 输出迭代器，接收 std::basic_string_view<TChar> 类型的结果
 * @param removeEmptyResult 是否移除分割结果中的空字符串，缺省为 true
 * @return 分割后得到了多少个子串
 */
template<typename T1, typename T2, typename TOutIterator, CONV_T(T1), CONV_T(T2), TCHAR_EQ(T1, T2)>
size_t Split(
    const T1 &_src, const T2 &_splitter,
    TOutIterator outIterator, bool removeEmptyResult = true)
{
    CONV(src);
    CONV(splitter);

    size_t beg = 0, ret = 0;
    while(beg < src.size())
    {
        size_t end = src.find(splitter, beg);

        if(end == std::basic_string_view<TCHAR(T1)>::npos)
        {
            ++ret;
            outIterator = src.substr(beg, src.size() - beg);
            ++outIterator;
            break;
        }

        if(end != beg || !removeEmptyResult)
        {
            ++ret;
            outIterator = src.substr(beg, end - beg);
            ++outIterator;
        }

        beg = end + splitter.size();
    }
    return ret;
}

/**
 * @brief 用空白字符作为分隔符分割字符串
 * @param _src 待分割的字符串，瑟瑟发抖中
 * @param outIterator 输出迭代器，接收 std::basic_string_view<TChar> 类型的结果
 * @param removeEmptyResult 是否移除分割结果中的空字符串，缺省为 true
 * @return 分割后得到了多少个子串
 */
template<typename T, typename TOutIterator, CONV_T(T)>
size_t Split(const T &_src, TOutIterator outIterator, bool removeEmptyResult = true)
{
    CONV(src);
    return Split(src, IsWhitespace<TCHAR(T)>, outIterator, removeEmptyResult);
}

/**
 * @brief 原地将字符串中的指定子串替换为另外的子串
 * @param str 被处理的母串
 * @param oldSubstr 被替换掉的子串
 * @param newSubstr 用来替换的新子串
 * @return 共替换掉了多少个子串
 */
template<typename TChar>
size_t ReplaceInPlace(
    std::basic_string<TChar> &str,
    const std::basic_string_view<TChar> &oldSubstr, const std::basic_string_view<TChar> &newSubstr)
{
    if(oldSubstr.empty())
        return 0;
    size_t ret = 0, pos = 0;
    while((pos = str.find(oldSubstr, pos)) != std::basic_string_view<TChar>::npos)
    {
        str.replace(pos, oldSubstr.size(), newSubstr);
        pos += newSubstr.size();
        ++ret;
    }
    return ret;
}

/**
 * @brief 给定一个字符串，返回将其中的指定子串替换为另外的子串后的结果
 * @param _str 被处理的母串
 * @param _oldSubstr 被替换掉的子串
 * @param _newSubstr 用来替换的新子串
 * @return 共替换掉了多少个子串
 */
template<typename T1, typename T2, typename T3, CONV_T(T1), CONV_T(T2), CONV_T(T3), TCHAR_EQ(T1, T2), TCHAR_EQ(T2, T3)>
std::basic_string<TCHAR(T1)> Replace(const T1 &_str, const T2 &_oldSubstr, const T3 &_newSubstr)
{
    CONV(str); CONV(oldSubstr); CONV(newSubstr);
    std::basic_string<TCHAR(T1)> ret(str);
    ReplaceInPlace(ret, oldSubstr, newSubstr);
    return ret;
}

AGZ_NEW_EXCEPTION(FromException);

namespace Impl
{
    template<typename TChar, typename T, typename...Args> struct ToImpl { };
    template<> struct ToImpl<char, int8_t>   { static std::string Call(int8_t   obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, int16_t>  { static std::string Call(int16_t  obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, int32_t>  { static std::string Call(int32_t  obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, int64_t>  { static std::string Call(int64_t  obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, uint8_t>  { static std::string Call(uint8_t  obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, uint16_t> { static std::string Call(uint16_t obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, uint32_t> { static std::string Call(uint32_t obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, uint64_t> { static std::string Call(uint64_t obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, float>    { static std::string Call(float    obj) { return std::to_string(obj); } };
    template<> struct ToImpl<char, double>   { static std::string Call(double   obj) { return std::to_string(obj); } };
    
    template<typename TChar> struct ToImpl<TChar, std::basic_string<TChar>>      { static std::basic_string<TChar> Call(const std::basic_string     <TChar> &obj) { return obj; } };
    template<typename TChar> struct ToImpl<TChar, std::basic_string_view<TChar>> { static std::basic_string<TChar> Call(const std::basic_string_view<TChar> &obj) { return obj; } };
    template<typename TChar> struct ToImpl<TChar, const TChar*>                  { static std::basic_string<TChar> Call(const TChar *obj)                         { return obj; } };
    template<typename TChar, size_t N> struct ToImpl<TChar, char[N]>             { static std::basic_string<TChar> Call(const char (&obj)[N])                     { return obj; } };

    template<typename TChar, typename T, typename...Args> struct FromImpl { };
    template<typename T> struct FromImpl<char, T>
    {
        static T Call(const std::string_view &str)
        {
            T ret;
            auto [np, er] = std::from_chars(str.data(), str.data() + str.size(), ret);
            if(er != std::errc() || np != str.data() + str.size())
                throw FromException(str.data());
            return ret;
        }
    };
} // namespace Impl

/**
 * @brief 将给定的对象转换为字符串
 * @param obj 待转换的对象
 * @param args 转换参数
 * @return 转换得到的字符串结果
 */
template<typename TChar, typename T, typename...Args>
std::basic_string<TChar> To(T &&obj, Args&&...args)
{
    return Impl::ToImpl<TChar, remove_rcv_t<T>, remove_rcv_t<Args>...>::Call(std::forward<T>(obj), std::forward<Args>(args)...);
}

/**
 * @brief 从字符串中parse出指定类型的对象
 * @param _src 待parse的字符串
 * @param args parsing过程的参数
 * @return parsing得到的结果
 */
template<typename T, typename T2, typename...Args, CONV_T(T2)>
T From(const T2 &_src, Args&&...args)
{
    CONV(src);
    return Impl::FromImpl<TCHAR(T2), remove_rcv_t<T>, remove_rcv_t<Args>...>::Call(src, std::forward<Args>(args)...);
}

namespace Impl
{
    template<typename TCodeUnit, typename TCodePoint = char32_t>
    using CU2UTF_t = std::conditional_t<
        sizeof(TCodeUnit) == 1,
        TUTF8<TCodeUnit, char32_t, true>,
        TUTF16<TCodeUnit, char32_t, true>>;
} // namespace Impl

/**
 * @brief 在一个UTF字符串后追加一个Unicode CodePoint
 * @param str 追加的字符串
 * @param cp 被追加的CodePoint
 */
template<typename TChar, typename TCodePoint>
std::basic_string<TChar> &AppendUnicodeCodePointInPlace(std::basic_string<TChar> &str, TCodePoint cp)
{
    using UTF = Impl::CU2UTF_t<TChar, TCodePoint>;

    TChar buf[UTF::MAX_CU_COUNT_IN_ONE_CP];
    size_t cuCount = UTF::Encode(cp, buf);

    for(size_t i = 0; i < cuCount; ++i)
        str.push_back(buf[i]);

    return str;
}

/**
 * @brief 在UTF字符串间进行编码转换
 * @tparam TCharIn 源字符串所使用的CodeUnit类型
 * @tparam TCharOut 目标字符串所使用的CodeUnit类型
 * @param str 源字符串
 * @return 转换得到的目标字符串
 * @exception UTFException 源字符串中包含不合法UTF序列时抛出
 */
template<typename TCharIn, typename TCharOut>
std::basic_string<TCharOut> ConvertBetweenUTF(const std::basic_string_view<TCharIn> &str)
{
    using UTFIn  = Impl::CU2UTF_t<TCharIn>;

    std::basic_string<TCharOut> ret;
    const TCharIn *pIn = str.data(), *end = str.data() + str.size();

    while(pIn < end)
    {
        auto [cp, newPIn] = UTFIn::Decode(pIn);
        AppendUnicodeCodePointInPlace(ret, cp);
        pIn = newPIn;
    }
    return ret;
}

#ifdef AGZ_OS_WIN32
using PlatformChar       = wchar_t;
using PlatformString     = std::wstring;
using PlatformStringView = std::wstring_view;
inline std::wstring Str2PStr(const std::string_view &str) { return ConvertBetweenUTF<char, wchar_t>(str); }
inline std::string PStr2Str(const std::wstring_view &str) { return ConvertBetweenUTF<wchar_t, char>(str); }
#else
using PlatformChar       = char;
using PlatformString     = std::string;
using PlatformStringView = std::string_view;
inline std::string_view Str2PStr(const std::string_view &str) { return str; }
inline std::string_view PStr2Str(const std::string_view &str) { return str; }
#endif

/**
 * @brief 以CodePoint的视角遍历一段UTF CodeUnit序列
 * @tparam TCodeUnit UTF CodeUnit类型
 * @tparam TCodePoint UTF CodePoint类型，缺省为char32_t
 */
template<typename TCodeUnit, typename TCodePoint = char32_t>
class UTFCodePointRange
{
    const TCodeUnit *beg_;
    const TCodeUnit *end_;

    using UTF       = Impl::CU2UTF_t<TCodeUnit, TCodePoint>;
    using CodeUnit  = TCodeUnit;
    using CodePoint = TCodePoint;

public:

    /**
     * @brief 用于遍历CodePoint的迭代器类型
     */
    class Iterator
    {
        const CodeUnit *pCodeUnit_;

        explicit Iterator(const CodeUnit *pCodeUnit) noexcept
            : pCodeUnit_(pCodeUnit)
        {
            AGZ_ASSERT(pCodeUnit);
        }

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type        = char32_t;
        using difference_type   = std::make_signed_t<size_t>;

        Iterator() noexcept
            : pCodeUnit_(nullptr)
        {
            
        }

        CodePoint operator*() const
        {
            auto [ret, np] = UTF::Decode(pCodeUnit_);
            return ret;
        }

        Iterator &operator++()
        {
            auto [cp, np] = UTF::Decode(pCodeUnit_);
            pCodeUnit_ = np;
            return *this;
        }

        Iterator operator++(int)
        {
            auto ret = *this;
            ++*this;
            return ret;
        }

        bool operator==(const Iterator &rhs) const noexcept
        {
            return pCodeUnit_ == rhs.pCodeUnit_;
        }

        bool operator!=(const Iterator &rhs) const noexcept
        {
            return pCodeUnit_ != rhs.pCodeUnit_;
        }
    };

    /**
     * @brief 默认初始化为空串
     */
    UTFCodePointRange() noexcept
        : beg_(nullptr), end_(nullptr)
    {
        
    }

    /**
     * @brief 以给定的范围初始化
     * @param beg 首个CodeUnit地址
     * @param end 范围后方第一个CodeUnit地址
     * CodeUnit地址范围为[beg, end)
     */
    UTFCodePointRange(const CodeUnit *beg, const CodeUnit *end) noexcept
        : beg_(beg), end_(end)
    {
        AGZ_ASSERT(beg && beg <= end);
    }

    /**
     * @brief 以给定的以0结尾的范围进行初始化
     * @param beg 首个CodeUnit地址
     * 设从beg开始，首个值为0的CodeUnit地址为end，则CodeUnit地址范围为[beg, end)
     */
    explicit UTFCodePointRange(const CodeUnit *beg) noexcept
        : beg_(beg), end_(beg)
    {
        AGZ_ASSERT(beg);
        while(*end_)
            ++end_;
    }

    /**
     * @brief 以标准库字符串进行初始化
     */
    explicit UTFCodePointRange(const std::basic_string<CodeUnit> &str) noexcept
        : beg_(str.data()), end_(str.data() + str.size())
    {
        
    }

    /**
     * @brief 以标准库字符串进行初始化
     */
    explicit UTFCodePointRange(const std::basic_string_view<CodeUnit> &str) noexcept
        : beg_(str.data()), end_(str.data() + str.size())
    {
        
    }

    /**
     * @brief 取得用于遍历CodePoint的起始迭代器
     */
    Iterator begin() const noexcept
    {
        return Iterator(beg_);
    }

    /**
     * @brief 取得用于遍历CodePoint的终止迭代器
     */
    Iterator end() const noexcept
    {
        return Iterator(end_);
    }
};

AGZ_NEW_EXCEPTION(FormatterException);

/**
 * @brief 字符串格式化工具
 *  
 *  使用时，在 TFormatter 的构造函数中给出格式化串，调用其实例的 Arg 方法得到输出串。如：
 *  
@code
auto str = TFormatter<char>("{0} + {0} = {}").Arg(2, 4);
assert(str == "2 + 2 = 4");
@endcode
 */
template<typename TChar>
class TFormatter
{
    struct Char    { TChar ch; };
    struct Seg     { std::basic_string<TChar> seg; };
    struct ArgIdx  { size_t idx; };

    using Unit = TypeOpr::Variant<Char, Seg, ArgIdx>;

    std::vector<Unit> units_;
    size_t maxArgIdx_;

    static void ArgArr2Str(std::basic_string<TChar> *) { }

    template<typename TArg, typename...TOthers>
    static void ArgArr2Str(std::basic_string<TChar> *pArr, TArg &&arg, TOthers&&...others)
    {
        *pArr = To<TChar>(std::forward<TArg>(arg));
        ArgArr2Str(pArr + 1, std::forward<TOthers>(others)...);
    }

public:

    /**
     * @brief 以格式串进行初始化
     * @param _fmt 格式串
     * 
     * 格式串语法：
     *  - {} 表示参数引用，引用下标从0开始，逐个递增
     *  - {N} 显式给定参数引用下标，且下一个默认下标为N+1
     *  - {{ 字符'{'的转义
     *  - 其他字符 直接输出
     * 
     * 格式串的解析较慢，若需要多次用同一个格式串进行字符串格式化，可以缓存该 TFormatter 对象以减小解析开销
     * 
     * @exception FormatterException 格式串含有语法错误时抛出
     */
    template<typename T, CONV_T(T), std::enable_if_t<std::is_same_v<TCHAR(T), TChar>, int> = 0>
    explicit TFormatter(const T &_fmt)
        : maxArgIdx_(0)
    {
        CONV(fmt);

        size_t cur = 0, nextArgIdx = 0;
        while(cur < fmt.size())
        {
            size_t braceIdx = fmt.find('{', cur);
            if(braceIdx == std::basic_string_view<TChar>::npos)
            {
                units_.emplace_back(Seg{ std::basic_string<TChar>(fmt.substr(cur)) });
                break;
            }

            if(braceIdx > cur)
                units_.emplace_back(Seg{ std::basic_string<TChar>(fmt.substr(cur, braceIdx - cur)) });

            if(braceIdx >= fmt.size() - 1)
                throw FormatterException("Invalid format string: '}' expected");

            if(fmt[braceIdx + 1] == '{')
            {
                units_.emplace_back(Char{ '{' });
                cur = braceIdx + 2;
                continue;
            }

            if(fmt[braceIdx + 1] == '}')
            {
                units_.emplace_back(ArgIdx{ nextArgIdx });
                maxArgIdx_ = std::max(maxArgIdx_, nextArgIdx);
                ++nextArgIdx;
                cur = braceIdx + 2;
                continue;
            }

            size_t argIdx;
            auto [newPCur, err] = std::from_chars(fmt.data() + braceIdx + 1, fmt.data() + fmt.size(), argIdx);
            if(err != std::errc())
                throw FormatterException("Invalid format string: unknown argument index");
            units_.emplace_back(ArgIdx{ argIdx });
            maxArgIdx_ = std::max(maxArgIdx_, argIdx);
            nextArgIdx = argIdx + 1;

            size_t endBracIdx = newPCur - fmt.data();
            if(endBracIdx >= fmt.size() || fmt[endBracIdx] != '}')
                throw FormatterException("Invalid format string: '}' expected");
            cur = endBracIdx + 1;
        }
    }

    /**
     * @brief 给出格式串中的参数引用内容，输出格式化后的字符串
     * @exception FormatterException 参数数量过少时抛出
     */
    template<typename...Args>
    std::basic_string<TChar> Arg(Args&&...args) const
    {
        if(maxArgIdx_ >= sizeof...(args))
            throw FormatterException("Invalid format arguments: sizeof...(args) must be more than " + To<TChar>(maxArgIdx_));

        std::basic_string<TChar> ret;
        std::array<std::basic_string<TChar>, sizeof...(args)> strs;
        ArgArr2Str(strs.data(), std::forward<Args>(args)...);

        for(auto &unit : units_)
        {
            TypeOpr::MatchVar(unit,
                [&](const Char &param)
            {
                ret.push_back(param.ch);
            },
                [&](const Seg &param)
            {
                ret.append(param.seg);
            },
                [&](const ArgIdx &param)
            {
                ret.append(strs[param.idx]);
            });
        }

        return ret;
    }
};

#undef CONV
#undef CONV_T
#undef TCHAR
#undef TCHAR_EQ

} // namespace AGZ::Str
