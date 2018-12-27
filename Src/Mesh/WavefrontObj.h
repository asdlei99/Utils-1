#pragma once

#include <limits>
#include <map>
#include <type_traits>

#include "../Utils/FileSys.h"
#include "../Math/Vec3.h"
#include "../Math/Vec4.h"
#include "../String/String/String.h"
#include "../Utils/Range.h"
#include "Mesh.h"

namespace AGZ::Mesh {

template<typename T>
class WavefrontObj
{
    static_assert(std::is_floating_point_v<T>, "Template parameter must be of floating-point type");

public:

    using Index = int32_t;
    static constexpr Index INDEX_NONE = (std::numeric_limits<Index>::max)();

    struct Object
    {
        struct Group
        {
            struct Face
            {
                bool isTriangle;

                struct FaceVertex
                {
                    Index pos;
                    Index tex;
                    Index nor;
                };
                FaceVertex v[4];
            };

            std::vector<Face> faces;
        };

        Option<const Group&> FindGroup(const Str8 &name) const
        {
            auto it = name2Group.find(name);
            if(it != name2Group.end())
                return Some(it->second);
            return None;
        }

        std::map<Str8, Group> name2Group;
    };

    Option<const Object&> FindObject(const Str8 &name) const
    {
        auto it = name2Obj.find(name);
        if(it != name2Obj.end())
            return Some(it->second);
        return None;
    }

    void Clear()
    {
        vtxPos.clear();
        vtxTex.clear();
        vtxNor.clear();
        name2Obj.clear();
    }

    bool LoadFromFile(const Str8 &filename)
    {
        Str8 content;
        if(!FileSys::ReadTextFileRaw(filename, &content))
            return false;
        return LoadFromMemory(content);
    }

    bool LoadFromMemory(const Str8 &content, bool ignoreUnknownLine = true) noexcept;

    GeometryMesh<T> ToGeometryMesh(
        const typename Object::Group &grp, bool reverseNor = false, bool reverseTex = false) const;

    GeometryMeshGroup<T> ToGeometryMeshGroup(bool reverseNor = false, bool reverseTex = false) const;

    std::vector<Math::Vec3<T>> vtxPos;
    std::vector<Math::Vec3<T>> vtxTex;
    std::vector<Math::Vec3<T>> vtxNor;

    std::map<Str8, Object> name2Obj;

private:

    static typename Object::Group::Face::FaceVertex ParseVertexIndex(const Str8 &s);
};

template<typename T>
bool WavefrontObj<T>::LoadFromMemory(const Str8 &content, bool ignoreUnknownLine) noexcept
{
    Clear();

    // 通过obj()来获取当前正在parse的object
    // 通过grp()来获取当前正在parse的group

    Object                  *_curObj = nullptr;
    typename Object::Group  *_curGrp = nullptr;

    auto obj = [&]() -> Object&
    {
        if(!_curObj) _curObj = &name2Obj["Default"];
        return *_curObj;
    };

    auto grp = [&]() -> typename Object::Group&
    {
        if(!_curGrp) _curGrp = &obj().name2Group["Default"];
        return *_curGrp;
    };

    try
    {
        // 按\n拆分并剔除空行、注释行

        auto lines = content.Split("\n")
            | FilterMap([](const Str8 &line) -> Option<Str8>
            {
                Str8 ret = line.Trim();
                if(ret.Empty() || ret.StartsWith("#"))
                    return None;
                return ret;
            })
            | Collect<std::vector<Str8>>();
        
        for(const auto &line : lines)
        {
            static thread_local Regex8 oReg(
                R"___(o\s+&@{!\s}+&)___");
            static thread_local Regex8 gReg(
                R"___(g\s+&@{!\s}+&)___");
            static thread_local Regex8 vReg(
                R"___(v\s+&@{!\s}+&\s+&@{!\s}+&\s+&@{!\s}+&(\s+@{!\s}+)?&)___");
            static thread_local Regex8 vtReg(
                R"___(vt\s+&@{!\s}+&\s+&@{!\s}+&(\s+@{!\s}+)?&)___");
            static thread_local Regex8 vnReg(
                R"___(vn\s+&@{!\s}+&\s+&@{!\s}+&\s+&@{!\s}+&)___");

            if(auto m = oReg.Match(line))
            {
                Str8 k(m(0, 1));
                name2Obj.erase(k);
                _curObj = &name2Obj[k];
                _curGrp = nullptr;
                continue;
            }

            if(auto m = gReg.Match(line))
            {
                Str8 k(m(0, 1));
                obj().name2Group.erase(k);
                _curGrp = &obj().name2Group[k];
                continue;
            }

            if(auto m = vReg.Match(line))
            {
                Math::Vec4<T> v = {
                    m(0, 1).template Parse<T>(),
                    m(2, 3).template Parse<T>(),
                    m(4, 5).template Parse<T>(),
                    m(5, 6).Empty() ? T(1) : m(5, 6).TrimLeft().template Parse<T>()
                };
                vtxPos.push_back(v.xyz() / v.w);
                continue;
            }

            if(auto m = vtReg.Match(line))
            {
                Math::Vec3<T> vt = {
                    m(0, 1).template Parse<T>(),
                    m(2, 3).template Parse<T>(),
                    m(3, 4).Empty() ? T(0) : m(3, 4).TrimLeft().template Parse<T>()
                };
                vtxTex.push_back(vt);
                continue;
            }

            if(auto m = vnReg.Match(line))
            {
                Math::Vec3<T> vn = {
                    m(0, 1).template Parse<T>(),
                    m(2, 3).template Parse<T>(),
                    m(4, 5).template Parse<T>()
                };
                vtxNor.push_back(vn);
                continue;
            }

            if(line.StartsWith("f"))
            {
                auto indices = line.Slice(1).Split();
                if(indices.size() < 3 || indices.size() > 4)
                    throw std::runtime_error("");
                
                typename Object::Group::Face face;
                for(size_t i = 0; i < indices.size(); ++i)
                {
                    auto v = ParseVertexIndex(indices[i]);
                    if(v.pos < 0) v.pos = Index(vtxPos.size()) + v.pos;
                    else if(v.pos != INDEX_NONE) --v.pos;
                    if(v.tex < 0) v.tex = Index(vtxTex.size()) + v.tex;
                    else if(v.tex != INDEX_NONE) --v.tex;
                    if(v.nor < 0) v.nor = Index(vtxNor.size()) + v.nor;
                    else if(v.nor != INDEX_NONE) --v.nor;
                    face.v[i] = v;
                }
                
                if(indices.size() == 3)
                    face.v[3].pos = face.v[3].tex = face.v[3].nor = INDEX_NONE;
                
                face.isTriangle = indices.size() != 4;
                
                grp().faces.push_back(face);
                continue;
            }

            if(!ignoreUnknownLine)
                throw std::runtime_error("");
        }
    }
    catch(...)
    {
        Clear();
        return false;
    }

    return true;
}

template<typename T>
typename WavefrontObj<T>::Object::Group::Face::FaceVertex
WavefrontObj<T>::ParseVertexIndex(const Str8 &str)
{
    typename Object::Group::Face::FaceVertex ret = { INDEX_NONE, INDEX_NONE, INDEX_NONE };

    static thread_local Regex8 reg0(R"___(-?\d+)___");
    if(reg0.Match(str))
    {
        ret.pos = str.Parse<int32_t>();
        return ret;
    }

    static thread_local Regex8 reg1(R"___(&-?\d+&/&-?\d+&)___");
    if(auto m = reg1.Match(str))
    {
        ret.pos = m(0, 1).Parse<int32_t>();
        ret.tex = m(2, 3).Parse<int32_t>();
        return ret;
    }

    static thread_local Regex8 reg2(R"___(&-?\d+&/&-?\d*&/&-?\d+&)___");
    if(auto m = reg2.Match(str))
    {
        ret.pos = m(0, 1).Parse<int32_t>();
        ret.tex = m(2, 3).Empty() ? INDEX_NONE : (m(2, 3).Parse<int32_t>());
        ret.nor = m(4, 5).Parse<int32_t>();
        return ret;
    }

    throw std::runtime_error("");
}

template<typename T>
GeometryMesh<T> WavefrontObj<T>::ToGeometryMesh(
        const typename Object::Group &grp, bool reverseNor, bool reverseTex) const
{
    std::vector<typename GeometryMesh<T>::Vertex> vtces;

    Math::Vec3<T> tB(T(1), T(0), T(0)), tC(T(0), T(1), T(0));
    if(reverseTex)
        std::swap(tB, tC);

    auto addTri = [&](const typename Object::Group::Face &f, const int (&js)[3])
    {
        size_t i = vtces.size();
        vtces.resize(vtces.size() + 3);

        for(int j = 0; j < 3; ++j)
            vtces[i + j].pos = vtxPos[f.v[js[j]].pos];

        if(f.v[js[0]].nor != INDEX_NONE && f.v[js[1]].nor != INDEX_NONE && f.v[js[2]].nor != INDEX_NONE)
        {
            for(int j = 0; j < 3; ++j)
                vtces[i + j].nor = vtxNor[f.v[js[j]].nor];
        }
        else
        {
            auto nor = Cross(
                vtces[i + 1].pos - vtces[i].pos, vtces[i + 2].pos - vtces[i].pos).Normalize();
            if(reverseNor)
                nor = -nor;
            for(int j = 0; j < 3; ++j)
                vtces[i + j].nor = nor;
        }

        if(f.v[js[0]].tex != INDEX_NONE && f.v[js[1]].tex != INDEX_NONE && f.v[js[2]].tex != INDEX_NONE)
        {
            for(int j = 0; j < 3; ++j)
                vtces[i + j].tex = vtxTex[f.v[js[j]].tex];
        }
        else
        {
            vtces[i].tex = Math::Vec3<T>(T(0));
            vtces[i + 1].tex = tB;
            vtces[i + 2].tex = tC;
        }
    };

    for(auto &f : grp.faces)
    {
        addTri(f, { 0, 1, 2 });
        if(!f.isTriangle)
            addTri(f, { 0, 2, 3 });
    }

    return GeometryMesh<T>{ vtces };
}

template<typename T>
GeometryMeshGroup<T> WavefrontObj<T>::ToGeometryMeshGroup(bool reverseNor, bool reverseTex) const
{
    std::map<Str8, GeometryMesh<T>> submeshes;
    for(auto &p : name2Obj)
    {
        for(auto &p2 : p.second.name2Group)
        {
            submeshes[p.first + "-" + p2.first] = ToGeometryMesh(p2.second, reverseNor, reverseTex);
        }
    }
    return GeometryMeshGroup<T>{ submeshes };
}

} // namespace AGZ::Mesh
