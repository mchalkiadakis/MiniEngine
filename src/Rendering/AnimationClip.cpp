#include "AnimationClip.h"
#include <gtc/matrix_transform.hpp>

int BoneChannel::GetPositionIndex(float time) const {
    for (int i = 0; i < (int)Positions.size() - 1; i++)
        if (time < Positions[i + 1].Time)
            return i;
    return (int)Positions.size() - 1;
}

int BoneChannel::GetRotationIndex(float time) const {
    for (int i = 0; i < (int)Rotations.size() - 1; i++)
        if (time < Rotations[i + 1].Time)
            return i;
    return (int)Rotations.size() - 1;
}

int BoneChannel::GetScaleIndex(float time) const {
    for (int i = 0; i < (int)Scales.size() - 1; i++)
        if (time < Scales[i + 1].Time)
            return i;
    return (int)Scales.size() - 1;
}

glm::mat4 BoneChannel::GetLocalTransform(float time) const {
    // PS1 style — snap to nearest keyframe, no interpolation
    glm::vec3 position = Positions.empty()
        ? glm::vec3(0.0f)
        : Positions[GetPositionIndex(time)].Position;

    glm::quat rotation = Rotations.empty()
        ? glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
        : Rotations[GetRotationIndex(time)].Rotation;

    glm::vec3 scale = Scales.empty()
        ? glm::vec3(1.0f)
        : Scales[GetScaleIndex(time)].Scale;

    return glm::translate(glm::mat4(1.0f), position)
        * glm::mat4_cast(rotation)
        * glm::scale(glm::mat4(1.0f), scale);
}