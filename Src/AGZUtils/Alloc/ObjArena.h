﻿#pragma once

/**
 * @file Alloc/ObjArena.h
 * @brief 定义了快速分配小对象的内存池ObjArena
 */

#include "../Misc/Common.h"
#include "../Misc/Exception.h"
#include "Alloc.h"

namespace AGZ {

/**
 * @brief 快速小对象分配器，允许分配构造任意类型的小对象，并支持统一释放
 *    
 * 空间用经典的chunklist管理，已分配对象由freelist管理。
 */
template<typename Alloc = CRTAllocator>
class ObjArena : public Uncopiable
{
    class DestructorInterface
    {
    public:

        DestructorInterface() noexcept = default;

        virtual ~DestructorInterface() = default;

        virtual size_t NodeSize() const noexcept = 0;

        virtual void Destruct() = 0;
    };

    template<typename T>
    class Destructor : public DestructorInterface
    {
    public:

        Destructor() noexcept = default;

        size_t NodeSize() const noexcept override
        {
            //return sizeof(T) + sizeof(NodeHead) + sizeof(Destructor<T>);
            return ObjArena<Alloc>::template Obj2NodeSize<T>();
        }

        void Destruct() override
        {
            T *dst = reinterpret_cast<T*>(
                reinterpret_cast<char*>(this) + sizeof(Destructor<T>));
            dst->~T();
        }
    };

    template<typename T>
    class ArrayDestructor : public DestructorInterface
    {
        size_t arrSize_;

    public:

        explicit ArrayDestructor(size_t arrSize) noexcept
            : arrSize_(arrSize)
        {
            
        }

        size_t NodeSize() const noexcept override
        {
            return ObjArena<Alloc>::template Arr2NodeSize<T>(arrSize_);
        }

        void Destruct() override
        {
            T *beg = reinterpret_cast<T*>(reinterpret_cast<char*>(this) + sizeof(ArrayDestructor<T>));
            for(size_t i = 0; i < arrSize_; ++i)
                (beg + i)->~T();
        }
    };

    struct ChunkHead
    {
        ChunkHead *nextChunk;
        char data[1];
    };

    struct NodeHead
    {
        NodeHead *nextNode;
    };

    ChunkHead *chunkEntry_;
    char *curChunkTop_;    // 当前chunk数据区的首个未占用字节地址
    size_t curChunkRest_;  // 当前chunk还剩多少字节

    NodeHead *nodeEntry_;

    const size_t chunkDataSize_;
    size_t usedBytes_;

    template<typename T>
    static size_t Obj2NodeSize() noexcept
    {
        return sizeof(T) + sizeof(NodeHead) + sizeof(Destructor<T>);
    }

    template<typename T>
    static size_t Arr2NodeSize(size_t arrSize) noexcept
    {
        return sizeof(NodeHead) + sizeof(ArrayDestructor<T>) + arrSize * sizeof(T);
    }

    void AllocNewChunk()
    {
        size_t size = sizeof(ChunkHead) + chunkDataSize_ - 1;
        char *data = static_cast<char*>(Alloc::Malloc(size));

        usedBytes_ += curChunkRest_ + size - chunkDataSize_;

        ChunkHead *head = reinterpret_cast<ChunkHead*>(data);
        head->nextChunk = chunkEntry_;
        chunkEntry_ = head;
        curChunkTop_ = head->data;
        curChunkRest_ = chunkDataSize_;
    }

public:

    /**
     * @param chunkDataSize 每次预分配块中有多少可用字节，默认为1024
     * 
     * @exception ArgumentException 参数非法时抛出
     */
    explicit ObjArena(size_t chunkDataSize = 1025 - sizeof(ChunkHead))
        : chunkEntry_(nullptr), curChunkTop_(nullptr), curChunkRest_(0),
          nodeEntry_(nullptr), chunkDataSize_(chunkDataSize), usedBytes_(0)
    {
        if(chunkDataSize < 1)
            throw ArgumentException("ObjArena: chunkDataSize must be positive");
    }

    ~ObjArena()
    {
        Clear();
    }

    /**
     * @brief 取得目前已使用的总字节数，包括簿记内存，但不包含预分配但未使用的内存
     */
    size_t GetUsedBytes() const noexcept
    {
        return usedBytes_;
    }

    /**
     * @brief 快速创建指定类型的对象
     * 
     * 如果对象过大，会将内存分配直接转发给Alloc；
     * 否则会尝试从预分配空间中取得内存。
     * 若预分配空间不足，会向Alloc申请更多的预分配内存。
     * 
     * @param args 被创建对象的构造函数参数
     * @return 指向被创建对象的指针
     * 
     * @exception std::bad_alloc 向Alloc请求更多内存空间失败时抛出
     */
    template<typename T, typename...Args>
    T *Create(Args&&...args)
    {
        // IMPROVE：对trivially_destructible的T，不需要Destructor

        size_t nodeSize = Obj2NodeSize<T>();

        // 过大的对象直接用Alloc::Malloc分配
        if(nodeSize > chunkDataSize_)
        {
            char *data = static_cast<char*>(Alloc::Malloc(nodeSize));
            char *destructor = data + sizeof(NodeHead);
            char *obj = destructor + sizeof(Destructor<T>);

            try
            {
                new(obj) T(std::forward<Args>(args)...);
            }
            catch(...)
            {
                Alloc::Free(data);
                throw;
            }

            new(destructor) Destructor<T>();

            NodeHead *newNode = reinterpret_cast<NodeHead*>(data);
            newNode->nextNode = nodeEntry_;
            nodeEntry_ = newNode;
            usedBytes_ += nodeSize;

            return reinterpret_cast<T*>(obj);
        }

        // 如果当前chunk剩余空间不足，就分配一个新chunk
        if(curChunkRest_ < nodeSize)
            AllocNewChunk();
        AGZ_ASSERT(curChunkRest_ >= nodeSize);

        char *data = curChunkTop_;
        char *destructor = data + sizeof(NodeHead);
        char *obj = destructor + sizeof(Destructor<T>);

        new(obj) T(std::forward<Args>(args)...);
        new(destructor) Destructor<T>();

        auto newNode = reinterpret_cast<NodeHead*>(data);
        newNode->nextNode = nodeEntry_;
        nodeEntry_ = newNode;

        curChunkTop_ += nodeSize;
        curChunkRest_ -= nodeSize;
        usedBytes_ += nodeSize;

        return reinterpret_cast<T*>(obj);
    }

    /**
     * @brief 在堆上快速创建指定类型的对象的数组
     *
     * 如果对象过大，会将内存分配直接转发给Alloc；
     * 否则会尝试从预分配空间中取得内存。
     * 若预分配空间不足，会向Alloc申请更多的预分配内存。
     *
     * 若对象构造过程中抛出异常，则已创建的对象会被析构，异常则会被原样抛出。
     *
     * @param arrSize 被创建数组的元素数量
     * @param args 被创建对象的构造函数参数
     * @return 指向被创建对象数组首元素的指针
     *
     * @exception std::bad_alloc 向Alloc请求更多内存空间失败时抛出
     */
    template<typename T, typename...Args>
    T *CreateArray(size_t arrSize, const Args&...args)
    {
        if(!arrSize)
            throw ArgumentException("ObjArena: alloc zero-sized array");
        size_t nodeSize = Arr2NodeSize<T>(arrSize);

        if(nodeSize > chunkDataSize_)
        {
            char *data       = static_cast<char*>(Alloc::Malloc(nodeSize));
            char *destructor = data + sizeof(NodeHead);
            char *obj        = destructor + sizeof(ArrayDestructor<T>);
            T *pObj          = reinterpret_cast<T*>(obj);

            size_t initEnd = 0;
            try
            {
                for(; initEnd < arrSize; ++initEnd)
                    new(pObj + initEnd) T(args...);
            }
            catch(...)
            {
                for(size_t i = 0; i < initEnd; ++i)
                    (pObj + i)->~T();
                Alloc::Free(data);
                throw;
            }

            new(destructor) ArrayDestructor<T>(arrSize);

            NodeHead *newNode = reinterpret_cast<NodeHead*>(data);
            newNode->nextNode = nodeEntry_;
            nodeEntry_        = newNode;
            usedBytes_        += nodeSize;

            return pObj;
        }

        if(curChunkRest_ < nodeSize)
            AllocNewChunk();
        AGZ_ASSERT(curChunkRest_ >= nodeSize);

        char *data       = curChunkTop_;
        char *destructor = data + sizeof(NodeHead);
        char *obj        = destructor + sizeof(ArrayDestructor<T>);
        T *pObj          = reinterpret_cast<T*>(obj);

        size_t initEnd = 0;
        try
        {
            for(; initEnd < arrSize; ++initEnd)
                new(pObj + initEnd) T(args...);
        }
        catch(...)
        {
            for(size_t i = 0; i < initEnd; ++i)
                (pObj + i)->~T();
            throw;
        }

        new(destructor) ArrayDestructor<T>(arrSize);

        NodeHead *newNode = reinterpret_cast<NodeHead*>(data);
        newNode->nextNode = nodeEntry_;
        nodeEntry_        = newNode;

        curChunkTop_  += nodeSize;
        curChunkRest_ -= nodeSize;
        usedBytes_    += nodeSize;

        return pObj;
    }

    /**
     * @brief 析构从上一次调用Clear以来所有用该分配器创建的对象，并释放它们的内存空间
     */
    void Clear()
    {
        // 析构所有已分配的对象
        for(NodeHead *nodeHead = nodeEntry_, *next; nodeHead; nodeHead = next)
        {
            next = nodeHead->nextNode;

            auto destructor = reinterpret_cast<DestructorInterface*>(
                reinterpret_cast<char*>(nodeHead) + sizeof(NodeHead));
            destructor->Destruct();
            size_t nodeSize = destructor->NodeSize();
            destructor->~DestructorInterface();
            if(nodeSize > chunkDataSize_)
                Alloc::Free(nodeHead);
        }

        // 释放所有chunk
        for(ChunkHead *chunkHead = chunkEntry_, *next; chunkHead; chunkHead = next)
        {
            next = chunkHead->nextChunk;
            Alloc::Free(chunkHead);
        }

        chunkEntry_ = nullptr;
        curChunkTop_ = nullptr;
        curChunkRest_ = 0;

        nodeEntry_ = nullptr;

        usedBytes_ = 0;
    }
};

} // namespace AGZ
