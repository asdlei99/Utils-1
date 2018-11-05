#pragma once

#include "../Misc/Common.h"
#include "../Misc/Exception.h"
#include "Alloc.h"

namespace AGZ {

/*
    只能分配固定大小内存块的Arena
*/
template<typename BaseAlloc = DefaultAllocator>
class FixedSizedArena
{
    struct Chunk
    {
        Chunk *next;
        char data[1];
    };

    struct Node
    {
        Node *next;
    };

    size_t nodeSize_;
    size_t chunkSize_;

    Node *freeNodes_;
    Chunk *chunkEntry_;

    void FreeAllImpl()
    {
        while(chunkEntry_)
        {
            Chunk *next = chunkEntry_->next;
            BaseAlloc::Free(chunkEntry_);
            chunkEntry_ = next;
        }
    }

public:

    explicit FixedSizedArena(size_t nodeSize)
        : FixedSizedArena(nodeSize, nodeSize * 32 + sizeof(Chunk*))
    {

    }

    FixedSizedArena(size_t nodeSize, size_t chunkByteSize)
        : nodeSize_(nodeSize), chunkSize_(chunkByteSize),
          freeNodes_(nullptr), chunkEntry_(nullptr)
    {
        if(nodeSize < sizeof(Node*) || nodeSize + sizeof(Chunk*) > chunkByteSize)
        {
            throw ArgumentException(
                "Invalid size arguments for FixedSizedArena");
        }
    }

    ~FixedSizedArena()
    {
        FreeAllImpl();
    }

    void *Alloc()
    {
        if(freeNodes_)
        {
            Node *ret = freeNodes_;
            freeNodes_ = freeNodes_->next;
            return ret;
        }

        auto *nChunk = reinterpret_cast<Chunk*>(BaseAlloc::Malloc(chunkSize_));
        nChunk->next = chunkEntry_;
        chunkEntry_ = nChunk;

        char *node = nChunk->data;
        char *end = reinterpret_cast<char*>(nChunk) + chunkSize_ - nodeSize_;
        while(node <= end)
        {
            Free(node);
            node += nodeSize_;
        }

        return Alloc();
    }

    void Free(void *ptr)
    {
        auto *n = reinterpret_cast<Node*>(ptr);
        n->next = freeNodes_;
        freeNodes_ = n;
    }

    void FreeAll()
    {
        FreeAllImpl();
    }
};

} // namespace AGZ
