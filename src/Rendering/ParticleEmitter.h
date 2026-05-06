#pragma once
#include <glm.hpp>
#include <vector>
#include <memory>
#include "Shader.h"
#include "Camera/Camera.h"

struct Particle {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec4 Color = glm::vec4(1.0f);
    float     Lifetime = 0.0f;
    float     MaxLife = 1.0f;
    float     Size = 1.0f;
    bool      Active = false;
};

struct ParticleVertex {
    glm::vec3 Position;
    glm::vec4 Color;
    float     Size;
};

class ParticleEmitter {
public:
    ParticleEmitter() = default;
    ~ParticleEmitter();

    ParticleEmitter(const ParticleEmitter&) = delete;
    ParticleEmitter& operator=(const ParticleEmitter&) = delete;

    ParticleEmitter(ParticleEmitter&& other) noexcept
        : m_Particles(std::move(other.m_Particles))
        , m_Position(other.m_Position)
        , m_EmissionRate(other.m_EmissionRate)
        , m_EmitAccum(other.m_EmitAccum)
        , m_MinLife(other.m_MinLife)
        , m_MaxLife(other.m_MaxLife)
        , m_StartSize(other.m_StartSize)
        , m_EndSize(other.m_EndSize)
        , m_Gravity(other.m_Gravity)
        , m_Emitting(other.m_Emitting)
        , m_StartColor(other.m_StartColor)
        , m_EndColor(other.m_EndColor)
        , m_VelMin(other.m_VelMin)
        , m_VelMax(other.m_VelMax)
        , m_VAO(other.m_VAO)
        , m_VBO(other.m_VBO)
    {
        other.m_VAO = 0;
        other.m_VBO = 0;
    }

    void Init(int maxParticles);

    void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    void SetEmissionRate(float rate) { m_EmissionRate = rate; }
    void SetLifetime(float minLife, float maxLife) { m_MinLife = minLife; m_MaxLife = maxLife; }
    void SetStartColor(const glm::vec4& c) { m_StartColor = c; }
    void SetEndColor(const glm::vec4& c) { m_EndColor = c; }
    void SetVelocityMin(const glm::vec3& v) { m_VelMin = v; }
    void SetVelocityMax(const glm::vec3& v) { m_VelMax = v; }
    void SetStartSize(float s) { m_StartSize = s; }
    void SetEndSize(float s) { m_EndSize = s; }
    void SetActive(bool active) { m_Emitting = active; }
    void SetGravity(float g) { m_Gravity = g; }

    void Update(float deltaTime);
    void Render(const Camera& camera, Shader& shader) const;

private:
    void SpawnParticle();
    int  FindDeadParticle() const;

    std::vector<Particle> m_Particles;
    glm::vec3             m_Position = glm::vec3(0.0f);

    float m_EmissionRate = 10.0f;
    float m_EmitAccum = 0.0f;
    float m_MinLife = 0.5f;
    float m_MaxLife = 1.5f;
    float m_StartSize = 1.0f;
    float m_EndSize = 0.0f;
    float m_Gravity = 0.0f;
    bool  m_Emitting = true;

    glm::vec4 m_StartColor = glm::vec4(1.0f, 0.4f, 0.0f, 1.0f);
    glm::vec4 m_EndColor = glm::vec4(1.0f, 0.8f, 0.0f, 0.0f);
    glm::vec3 m_VelMin = glm::vec3(-0.5f, 1.0f, -0.5f);
    glm::vec3 m_VelMax = glm::vec3(0.5f, 3.0f, 0.5f);

    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
};