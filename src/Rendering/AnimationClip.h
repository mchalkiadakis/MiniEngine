#pragma once
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <unordered_map>

struct KeyPosition {
    glm::vec3 Position;
    float     Time;
};

struct KeyRotation {
    glm::quat Rotation;
    float     Time;
};

struct KeyScale {
    glm::vec3 Scale;
    float     Time;
};

struct BoneChannel {
    std::string              BoneName;
    std::vector<KeyPosition> Positions;
    std::vector<KeyRotation> Rotations;
    std::vector<KeyScale>    Scales;

    glm::mat4 GetLocalTransform(float time) const;

private:
    int GetPositionIndex(float time) const;
    int GetRotationIndex(float time) const;
    int GetScaleIndex(float time)    const;
};

struct AnimationClip {
    std::string                              Name;
    float                                    Duration = 0.0f;
    float                                    TicksPerSec = 24.0f;
    std::unordered_map<std::string,
        BoneChannel>          Channels;
};