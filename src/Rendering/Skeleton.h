#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <unordered_map>


struct BoneInfo {
    int       Id;
    glm::mat4 OffsetMatrix; // transforms vertex from model space to bone space
};

struct Bone {
    std::string Name;
    int         ParentIndex = -1;
    int         OriginalId = -1; // ID from boneInfoMap (matches vertex BoneIds)
    glm::mat4   LocalTransform{ 1.0f };
    glm::mat4   OffsetMatrix{ 1.0f };
};

class Skeleton {
public:
    int  AddBone(const std::string& name, int parentIndex,
        const glm::mat4& offsetMatrix);
    int  GetBoneIndex(const std::string& name) const;
    int  GetBoneCount() const { return (int)m_Bones.size(); }
    Bone& GetBone(int index) { return m_Bones[index]; }
    const Bone& GetBone(int index) const { return m_Bones[index]; }

    std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() {
        return m_BoneInfoMap;
    }

private:
    std::vector<Bone>                         m_Bones;
    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
};