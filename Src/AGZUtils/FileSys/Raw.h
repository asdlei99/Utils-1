﻿#pragma once

#include <fstream>
#include <sstream>
#include <string_view>

#include "../Alloc/Malloc.h"
#include "../Misc/Common.h"
#include "../String/StdStr.h"

namespace AGZ::FileSys {
    
template<typename AllocFunc = void*(*)(size_t)>
std::pair<size_t, unsigned char*> ReadBinaryFileRaw(std::string_view filename, AllocFunc &&func = &(std::malloc))
{
    std::ifstream fin(WIDEN(filename).c_str(),
                      std::ios::in | std::ios::binary);
    if(!fin)
        return { 0, nullptr };

    fin.seekg(0, std::ios::end);
    auto len = fin.tellg();
    fin.seekg(0, std::ios::beg);

    auto buf = alloc_throw<unsigned char>(
        std::forward<AllocFunc>(func), static_cast<size_t>(len));
    fin.read(reinterpret_cast<char*>(buf), len);

    return { static_cast<size_t>(len), buf };
}

inline void DefaultlyReleaseRawBinaryFileContent(unsigned char *ptr)
{
    std::free(ptr);
}

inline bool WriteBinaryFileRaw(std::string_view filename, const unsigned char *data, size_t len)
{
    std::ofstream fout(WIDEN(filename).c_str(),
                       std::ios::binary | std::ios::trunc);
    if(!fout)
        return false;
    fout.write(reinterpret_cast<const char*>(data), len);
    fout.close();
    return true;
}

inline bool ReadTextFileRaw(std::string_view filename, std::string *str)
{
    std::ifstream fin(WIDEN(filename), std::ios_base::in);
    if(!fin)
        return false;
    std::stringstream sst;
    sst << fin.rdbuf();
    *str = sst.str();
    return true;
}

inline bool WriteTextFileRaw(std::string_view filename, std::string_view str)
{
    std::ofstream fout(WIDEN(filename), std::ios_base::out | std::ios_base::trunc);
    if(!fout)
        return false;
    fout << str;
    return true;
}

/**
 * @brief 对整个文件一次进行读写的便利操作
 */
class WholeFile
{
public:

    /**
     * 一次读取整个二进制文件的内容
     *
     * @param filename 文件路径
     * @param func 用来分配存储文件内容的空间的函数
     *
     * @note 若func参数使用缺省值，则这块空间应使用WholeFile::DefaultlyReleaseBinaryContent释放
     *
     * @return 返回二元组(字节数，内容指针)，读取失败时内容指针为空
     */
    template<typename AllocFunc = void*(*)(size_t)>
    std::pair<size_t, unsigned char*> ReadBinary(
        std::string_view filename, AllocFunc &&func = &(std::malloc))
    {
        return ReadBinaryFileRaw(filename, std::forward<AllocFunc>(func));
    }

    /**
     * 若调用WholeFile::ReadBinary时使用了缺省分配函数，则内容指针应用该函数释放
     */
    static void DefaultlyReleaseBinaryContent(unsigned char *ptr)
    {
        return DefaultlyReleaseRawBinaryFileContent(ptr);
    }

    /**
     * 一次写入整个二进制文件
     *
     * @param filename 文件路径
     * @param data 待写入的数据指针，不得为空
     * @param len 待写入的数据字节数
     *
     * @return 若写入遇到错误，返回false
     */
    static bool WriteBinaryFile(
        std::string_view filename, const unsigned char *data, size_t len)
    {
        return WriteBinaryFileRaw(filename, data, len);
    }

    /**
     * 一次读取整个文本文件
     *
     * @param filename 文件路径
     * @param str 用于输出的字符串，不得为空
     *
     * @return 读取失败时返回false
     */
    static bool ReadText(std::string_view filename, std::string *str)
    {
        return ReadTextFileRaw(filename, str);
    }

    /**
     * 一次写入整个文本文件
     *
     * @param filename 文件路径
     * @param str 待写入的字符串
     *
     * @return 若写入遇到错误，返回false
     */
    static bool WriteText(std::string_view filename, std::string_view str)
    {
        return WriteTextFileRaw(filename, str);
    }
};

} // namespace AGZ::FileSys
