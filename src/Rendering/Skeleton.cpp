#include "Skeleton.h"

int Skeleton::AddBone(const std::string& name, int parentIndex,
    const glm::mat4& offsetMatrix) {
    int index = (int)m_Bones.size();
    Bone bone;
    bone.Name = name;
    bone.ParentIndex = parentIndex;
    bone.OffsetMatrix = offsetMatrix;
    m_Bones.push_back(bone);

    BoneInfo info;
    info.Id = index;
    info.OffsetMatrix = offsetMatrix;
    m_BoneInfoMap[name] = info;

    return index;
}

int Skeleton::GetBoneIndex(const std::string& name) const {
    auto it = m_BoneInfoMap.find(name);
    if (it != m_BoneInfoMap.end())
        return it->second.Id;
    return -1;
}