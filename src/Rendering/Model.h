#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Mesh.h"
#include "Material.h"
#include "Skeleton.h"
#include "AnimationClip.h"
#include "Rendering/RenderContext.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Model {
public:
    struct SubMesh {
        std::unique_ptr<Mesh>     mesh;
        std::unique_ptr<Material> material;
    };

    void AddSubMesh(std::unique_ptr<Mesh> mesh,
        std::unique_ptr<Material> material);
    void Draw(const RenderContext& ctx,
        const glm::mat4& modelMatrix) const;
    void DrawGeometry() const;

private:
    std::vector<SubMesh> m_SubMeshes;
};

class SkinnedModel {
public:
    struct SkinnedSubMesh {
        std::unique_ptr<SkinnedMesh> mesh;
        std::unique_ptr<Material>    material;
    };

    void AddSubMesh(std::unique_ptr<SkinnedMesh> mesh,
        std::unique_ptr<Material> material);

    void Draw(const RenderContext& ctx,
        const glm::mat4& modelMatrix,
        const std::vector<glm::mat4>& boneMatrices) const;

    void DrawGeometry() const;

    void SetSkeleton(std::shared_ptr<Skeleton> skeleton) {
        m_Skeleton = skeleton;
    }
    void AddClip(const AnimationClip& clip) {
        m_Clips.push_back(clip);
    }

    std::shared_ptr<Skeleton>           GetSkeleton() const { return m_Skeleton; }
    const std::vector<AnimationClip>& GetClips()    const { return m_Clips; }
    void SetTexture(std::shared_ptr<Texture> tex) {
        for (auto& sub : m_SubMeshes)
            sub.material = std::make_unique<Material>(
                sub.material->GetShader(), tex);
    }

    void SetGlobalInverseTransform(const glm::mat4& m) {
        m_GlobalInverseTransform = m;
    }
    const glm::mat4& GetGlobalInverseTransform() const {
        return m_GlobalInverseTransform;
    }
    
private:
    glm::mat4 m_GlobalInverseTransform{ 1.0f };

private:
    std::vector<SkinnedSubMesh>  m_SubMeshes;
    std::shared_ptr<Skeleton>    m_Skeleton;
    std::vector<AnimationClip>   m_Clips;
};