#pragma once
#include "Skeleton.h"
#include "AnimationClip.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

class Animator {
public:
    Animator() = default;

    void AddClip(const AnimationClip& clip);
    void Play(const std::string& clipName);
    void Update(float deltaTime);
    void SetGlobalInverseTransform(const glm::mat4& m) {
        m_GlobalInverseTransform = m;
    }
    // returns final bone matrices ready to upload to shader
    const std::vector<glm::mat4>& GetBoneMatrices() const {
        return m_FinalMatrices;
    }

    void SetSkeleton(std::shared_ptr<Skeleton> skeleton) {
        m_Skeleton = skeleton;
        m_FinalMatrices.resize(skeleton->GetBoneCount(), glm::mat4(1.0f));
    }
    void ResizeBoneMatrices(int size) {
        m_FinalMatrices.resize(size, glm::mat4(1.0f));
    }
    void SetFPS(float fps) { m_FPS = fps; }
    bool HasClip(const std::string& name) const;

private:
    void ComputeBoneMatrices(const AnimationClip& clip,
        float time,
        int boneIndex,
        const glm::mat4& parentTransform);
    glm::mat4 m_GlobalInverseTransform{ 1.0f };
    std::shared_ptr<Skeleton>                      m_Skeleton;
    std::unordered_map<std::string, AnimationClip> m_Clips;
    std::string                                    m_CurrentClip;
    float                                          m_CurrentTime = 0.0f;
    float                                          m_FPS = 8.0f; // PS1 feel
    std::vector<glm::mat4>                         m_FinalMatrices;
};