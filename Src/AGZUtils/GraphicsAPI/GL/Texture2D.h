﻿#pragma once

#include "../../Utils/Texture.h"
#include "Common.h"

namespace AGZ::GraphicsAPI::GL
{

namespace Impl
{
    template<typename DataPixel> struct PT2DT;

    template<> struct PT2DT<GLfloat> { static constexpr GLenum format = GL_RED;  static constexpr GLenum type = GL_FLOAT;         static constexpr GLint rowAlignment = 4; };
    template<> struct PT2DT<Vec3f>   { static constexpr GLenum format = GL_RGB;  static constexpr GLenum type = GL_FLOAT;         static constexpr GLint rowAlignment = 4; };
    template<> struct PT2DT<Vec4f>   { static constexpr GLenum format = GL_RGBA; static constexpr GLenum type = GL_FLOAT;         static constexpr GLint rowAlignment = 4; };
    template<> struct PT2DT<GLubyte> { static constexpr GLenum format = GL_RED;  static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint rowAlignment = 1; };
    template<> struct PT2DT<Vec3b>   { static constexpr GLenum format = GL_RGB;  static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint rowAlignment = 1; };
    template<> struct PT2DT<Vec4b>   { static constexpr GLenum format = GL_RGBA; static constexpr GLenum type = GL_UNSIGNED_BYTE; static constexpr GLint rowAlignment = 4; };
}

/**
 * @brief 对OpenGL二维纹理对象的封装
 */
class Texture2D : public GLObject
{
public:

    /**
     * @param initHandle 是否立即创建一个GL Texture2D Name
     */
    explicit Texture2D(bool initHandle = false) noexcept
    {
        if(initHandle)
            InitializeHandle();
    }

    Texture2D(Texture2D &&moveFrom) noexcept
        : GLObject(moveFrom.handle_)
    {
        moveFrom.handle_ = 0;
    }

    Texture2D &operator=(Texture2D &&moveFrom) noexcept
    {
        Destroy();
        std::swap(handle_, moveFrom.handle_);
        return *this;
    }

    ~Texture2D()
    {
        Destroy();
    }

    /**
     * @brief 若含有Texture2D Object，将该Buffer标记为删除
     */
    void Destroy() noexcept
    {
        if(handle_)
        {
            AGZ_GL_CTX glDeleteTextures(1, &handle_);
            handle_ = 0;
        }
    }

    /**
     * @brief 创建一个GL Texture2D Name
     * @note 若已创建一个Name且未经删除，则调用该函数的结果是未定义的
     */
    void InitializeHandle() noexcept
    {
        AGZ_ASSERT(!handle_);
        AGZ_GL_CTX glCreateTextures(GL_TEXTURE_2D, 1, &handle_);
    }

    /**
     * @brief 初始化该纹理对象的格式和内容
     * 
     * 相当于InitializeFormat + ReinitializeData一次完成
     * 
     * @param levels MipMap级数
     * @param width 纹理宽度
     * @param height 纹理高度
     * @param internalFormat 纹理内部像素格式，参见 glTextureStorage2D
     * @param data 用于初始化的数据
     */
    template<typename DataTexelType>
    void InitializeFormatAndData(
        GLsizei levels, GLsizei width, GLsizei height, GLenum internalFormat, const DataTexelType *data) const noexcept
    {
        InitializeFormat(levels, width, height, internalFormat);
        ReinitializeData(width, height, data);
    }

    /**
     * @brief 初始化该纹理对象的格式和内容
     *
     * 相当于InitializeFormat + ReinitializeData一次完成
     *
     * @param levels MipMap级数
     * @param internalFormat 纹理内部像素格式，参见 glTextureStorage2D
     * @param data 用于初始化的纹理数据
     */
    template<typename DataTexelType>
    void InitializeFormatAndData(
        GLsizei levels, GLenum internalFormat, const TextureCore<2, DataTexelType> &data) const noexcept
    {
        AGZ_ASSERT(handle_ && data.IsAvailable());
        InitializeFormatAndData(levels, data.GetSize()[0], data.GetSize()[1], internalFormat, data.RawData());
    }

    /**
     * @brief 初始化内部像素格式，除非被Destroy，否则只能调用一次
     * @param levels MipMap级数
     * @param width 纹理宽度
     * @param height 纹理高度
     * @param internalFormat 纹理内部像素格式，参见 glTextureStorage2D
     */
    void InitializeFormat(GLsizei levels, GLsizei width, GLsizei height, GLenum internalFormat) const noexcept
    {
        AGZ_ASSERT(handle_);
        AGZ_GL_CTX glTextureStorage2D(handle_, levels, internalFormat, width, height);
    }

    /**
     * @brief 重新设置纹理内容
     * 
     * 只能在InitializeFormatAndData或InitializeFormat之后调用，可重复调用多次
     * 
     * @param width 纹理宽度
     * @param height 纹理高度
     * @param data 用于初始化的数据
     */
    template<typename DataTexelType>
    void ReinitializeData(GLsizei width, GLsizei height, const DataTexelType *data) const noexcept
    {
        AGZ_ASSERT(handle_);
        GLint oldAlignment;
        AGZ_GL_CTX glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, Impl::PT2DT<DataTexelType>::rowAlignment);
        glTextureSubImage2D(
            handle_, 0, 0, 0, width, height,
            Impl::PT2DT<DataTexelType>::format, Impl::PT2DT<DataTexelType>::type, data);
        AGZ_GL_CTX glPixelStorei(GL_UNPACK_ALIGNMENT, oldAlignment);
        AGZ_GL_CTX glGenerateTextureMipmap(handle_);
    }

    /**
     * @brief 设置纹理参数，见 glTextureParameteri
     */
    void SetParameter(GLenum paramName, GLint paramValue) const noexcept
    {
        AGZ_ASSERT(handle_);
        AGZ_GL_CTX glTextureParameteri(handle_, paramName, paramValue);
    }

    /**
     * @brief 绑定到指定的Texture Unit
     */
    void Bind(GLuint textureUnit) const noexcept
    {
        AGZ_ASSERT(handle_);
        AGZ_GL_CTX glBindTextureUnit(textureUnit, handle_);
    }

    /**
     * @brief 从指定的Texture Unit解绑
     */
    void Unbind(GLuint textureUnit) const noexcept
    {
        AGZ_ASSERT(handle_);
        AGZ_GL_CTX glBindTextureUnit(textureUnit, 0);
    }
};

} // namespace AGZ::GraphicsAPI::GL
