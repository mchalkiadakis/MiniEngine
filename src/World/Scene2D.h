#pragma once
#include "IScene.h"
#include "Actor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <memory>
#include <stdexcept>

class Scene2D : public IScene {
public:
    void Update(float deltaTime, Camera& camera) override {};
    void Render(const RenderContext& ctx) const override;
    void RenderDepth(Shader& depthShader,
        const glm::mat4& lightSpaceMatrix) const override {
    }

    void AddPointLight(const PointLight& light) override {}
    const std::vector<PointLight>& GetPointLights() const override {
        static std::vector<PointLight> empty;
        return empty;
    }
    Entity& CreateEntity(const std::string& name) override {
        throw std::runtime_error("Scene2D does not support entities");
    }
    Entity* GetEntity(const std::string& name) override { return nullptr; }
    Actor& CreateActor(const std::string& name,
        float maxHealth = 100.0f) override {
        throw std::runtime_error("Scene2D does not support actors");
    }
    void AddActor(std::unique_ptr<Actor> actor) override {}

    std::unique_ptr<IScene> GetNextScene() override {
        return std::move(m_NextScene);
    }

protected:
    virtual void OnRenderUI() = 0; // const — matches Render() const

    void RequestTransition(std::unique_ptr<IScene> next) {
        m_NextScene = std::move(next);
    }

private:
    std::unique_ptr<IScene> m_NextScene;
};