#pragma once
#include <memory>
#include <vector>
#include <string>
#include "IScene.h"
#include "ChunkManager.h"
#include "Skybox.h"
#include "Rendering/Light.h"
#include "Rendering/PointLight.h"

class Entity;

class Scene : public IScene {
public:
    Entity& CreateEntity(const std::string& name);
    void SetChunkManager(std::unique_ptr<ChunkManager> chunkManager);
    void SetSkybox(std::unique_ptr<Skybox> skybox);
    void Update(float deltaTime, const Camera& camera) override;
    void Render(const RenderContext& ctx) const override;
    void AddPointLight(const PointLight& light) override;
    const std::vector<PointLight>& GetPointLights() const override;

private:
    std::vector<std::unique_ptr<Entity>> m_Entities;
    std::unique_ptr<ChunkManager>        m_ChunkManager;
    std::unique_ptr<Skybox>              m_Skybox;
    std::vector<PointLight>              m_PointLights;
};