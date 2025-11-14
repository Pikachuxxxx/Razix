#pragma once

#include "Razix/Core/Containers/arrays.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cereal/cereal.hpp>
#include <spdlog/spdlog.h>

namespace glm {
    // glm vectors
    template<class Archive>
    void serialize(Archive& archive, float2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, float3& v)
    {
        archive(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z));
    }
    template<class Archive>
    void serialize(Archive& archive, float4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template<class Archive>
    void serialize(Archive& archive, int2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, int3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template<class Archive>
    void serialize(Archive& archive, int4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template<class Archive>
    void serialize(Archive& archive, uint2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, uint3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template<class Archive>
    void serialize(Archive& archive, uint4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }
    template<class Archive>
    void serialize(Archive& archive, dvec2& v)
    {
        archive(v.x, v.y);
    }
    template<class Archive>
    void serialize(Archive& archive, dvec3& v)
    {
        archive(v.x, v.y, v.z);
    }
    template<class Archive>
    void serialize(Archive& archive, dvec4& v)
    {
        archive(v.x, v.y, v.z, v.w);
    }

    // glm matrices
    template<class Archive>
    void serialize(Archive& archive, float2x2& m)
    {
        archive(m[0], m[1]);
    }
    template<class Archive>
    void serialize(Archive& archive, dmat2& m)
    {
        archive(m[0], m[1]);
    }
    template<class Archive>
    void serialize(Archive& archive, float3x3& m)
    {
        archive(m[0], m[1], m[2]);
    }
    template<class Archive>
    void serialize(Archive& archive, float4x4& m)
    {
        archive(m[0], m[1], m[2], m[3]);
    }
    template<class Archive>
    void serialize(Archive& archive, dmat4& m)
    {
        archive(m[0], m[1], m[2], m[3]);
    }

    // glm quats
    template<class Archive>
    void serialize(Archive& archive, quat& q)
    {
        archive(q.x, q.y, q.z, q.w);
    }
    template<class Archive>
    void serialize(Archive& archive, dquat& q)
    {
        archive(q.x, q.y, q.z, q.w);
    }
}    // namespace glm

namespace cereal {
    // Serialization for RZDynamicArray
    template<class Archive, typename T>
    void serialize(Archive& archive, Razix::RZDynamicArray<T>& arr)
    {
        // Get the size
        size_t size = arr.size();

        // Serialize size
        archive(cereal::make_size_tag(size));

        // On load, resize the array
        if (Archive::is_loading::value) {
            arr.clear();
            arr.reserve(size);
        }

        // Serialize each element
        for (size_t i = 0; i < size; ++i) {
            archive(arr[i]);
        }
    }
}    // namespace cereal

//namespace Razix {
//    template<class Archive>
//    void save(Archive& ar, RZString const& str)
//    {
//        std::string temp(str.c_str());
//        ar(temp);
//    }
//
//    template<class Archive>
//    void load(Archive& ar, RZString& str)
//    {
//        std::string temp;
//        ar(temp);
//        str = RZString(temp.c_str());
//    }
//}    // namespace Razix