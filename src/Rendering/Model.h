#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Mesh.h"
#include "Material.h"
#include "Camera/Camera.h"
#include "Rendering/Light.h"
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

    void AddSubMesh(std::unique_ptr<Mesh> mesh, std::unique_ptr<Material> material);
    void Draw(const RenderContext& ctx, const glm::mat4& modelMatrix) const;
    //draw your geometry, but don't touch the shader
    void DrawGeometry() const; // depth pass — no material bind

private:
    std::vector<SubMesh> m_SubMeshes;
};