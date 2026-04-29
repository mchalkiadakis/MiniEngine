#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include "Camera/Camera.h"
#include "Shader.h"
#include "PointLight.h"
#include "Light.h"
#include "Effects/FogSettings.h"

struct RenderContext {
    const Camera& camera;
    const DirectionalLight& light;
    const std::vector<PointLight>& pointLights;
    const FogSettings& fog;
    glm::mat4                      lightSpaceMatrix{ 1.0f };
    int                            shadowMapUnit = 4;

    void ApplyToShader(Shader& shader, const glm::mat4& modelMatrix) const {
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

        shader.SetUniformMat4("u_Model", glm::value_ptr(modelMatrix));
        shader.SetUniformMat4("u_View", glm::value_ptr(camera.GetViewMatrix()));
        shader.SetUniformMat4("u_Projection", glm::value_ptr(camera.GetProjectionMatrix()));
        shader.SetUniformMat3("u_NormalMatrix", glm::value_ptr(normalMatrix));
        shader.SetUniform3f("u_LightDir", light.Direction);
        shader.SetUniform3f("u_LightColor", light.Color);
        shader.SetUniform3f("u_ViewPos", camera.GetPosition());
        shader.SetPointLights(pointLights);
        shader.SetUniform3f("u_FogColor", fog.Color);
        shader.SetUniform1f("u_FogDensity", fog.Density);
        shader.SetUniform1i("u_FogEnabled", fog.Enabled ? 1 : 0);

        // shadow map
        shader.SetUniformMat4("u_LightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        shader.SetUniform1i("u_ShadowMap", shadowMapUnit);
       // shader.SetUniform1i("u_NormalMap", 1); // texture unit 1
       // shader.SetUniform1i("u_UseNormalMap", 0); // off by default
    }
};