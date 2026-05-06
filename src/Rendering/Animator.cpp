#include "Animator.h"
#include <gtc/matrix_transform.hpp>
#include <iostream>


void Animator::AddClip(const AnimationClip& clip) {
    m_Clips[clip.Name] = clip;
}

void Animator::Play(const std::string& clipName) {
    if (m_CurrentClip == clipName) return;
    if (m_Clips.find(clipName) == m_Clips.end()) {
        std::cerr << "Animation clip not found: " << clipName << "\n";
        return;
    }
    m_CurrentClip = clipName;
    m_CurrentTime = 0.0f;
}

bool Animator::HasClip(const std::string& name) const {
    return m_Clips.find(name) != m_Clips.end();
}

void Animator::Update(float deltaTime) {
    if (m_CurrentClip.empty() || !m_Skeleton) return;
    if (m_Skeleton->GetBoneCount() == 0) return;
    if (m_FinalMatrices.empty()) return;

    const AnimationClip& clip = m_Clips.at(m_CurrentClip);

    m_CurrentTime += m_FPS * deltaTime;
    if (m_CurrentTime >= clip.Duration)
        m_CurrentTime = fmod(m_CurrentTime, clip.Duration);

    std::fill(m_FinalMatrices.begin(), m_FinalMatrices.end(), glm::mat4(1.0f));
    ComputeBoneMatrices(clip, m_CurrentTime, 0, glm::mat4(1.0f));
}

void Animator::ComputeBoneMatrices(const AnimationClip& clip,
    float time,
    int boneIndex,
    const glm::mat4& parentTransform) {

    if (!m_Skeleton) return;
    if (boneIndex < 0 || boneIndex >= m_Skeleton->GetBoneCount()) return;
    if (boneIndex >= (int)m_FinalMatrices.size()) return;

    const Bone& bone = m_Skeleton->GetBone(boneIndex);
    glm::mat4 localTransform = bone.LocalTransform;

    auto it = clip.Channels.find(bone.Name);
    if (it != clip.Channels.end()) {
        localTransform = it->second.GetLocalTransform(time);

        // strip root motion from hips and scene root
        if (bone.ParentIndex == -1 ||
            bone.Name == "mixamorig:Hips" ||
            bone.Name == "Hips") {
            localTransform[3][0] = 0.0f;
            localTransform[3][1] = 0.0f;
            localTransform[3][2] = 0.0f;
        }
    }

    glm::mat4 globalTransform = parentTransform * localTransform;

    if (bone.OriginalId >= 0 && bone.OriginalId < (int)m_FinalMatrices.size()) {
        m_FinalMatrices[bone.OriginalId] = m_GlobalInverseTransform
            * globalTransform
            * bone.OffsetMatrix;
    }

    for (int i = 0; i < m_Skeleton->GetBoneCount(); i++) {
        if (i < 0 || i >= (int)m_FinalMatrices.size()) continue;
        if (m_Skeleton->GetBone(i).ParentIndex == boneIndex)
            ComputeBoneMatrices(clip, time, i, globalTransform);
    }
}