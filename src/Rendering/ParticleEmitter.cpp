#include "ParticleEmitter.h"
#include <glad/glad.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <random>

static std::mt19937 s_Rng{ std::random_device{}() };

static float RandFloat(float min, float max) {
    return std::uniform_real_distribution<float>(min, max)(s_Rng);
}


ParticleEmitter::~ParticleEmitter() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void ParticleEmitter::Init(int maxParticles) {
    m_Particles.resize(maxParticles);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // allocate space for max particles — updated every frame
    glBufferData(GL_ARRAY_BUFFER,
        maxParticles * sizeof(ParticleVertex),
        nullptr, GL_DYNAMIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, Position));
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
        sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, Color));
    glEnableVertexAttribArray(1);

    // size
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
        sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, Size));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

int ParticleEmitter::FindDeadParticle() const {
    for (int i = 0; i < (int)m_Particles.size(); i++)
        if (!m_Particles[i].Active)
            return i;
    return -1;
}

void ParticleEmitter::SpawnParticle() {
    int idx = FindDeadParticle();
    if (idx < 0) return;

    Particle& p = m_Particles[idx];
    p.Active = true;
    p.Position = m_Position;
    p.Velocity = glm::vec3(
        RandFloat(m_VelMin.x, m_VelMax.x),
        RandFloat(m_VelMin.y, m_VelMax.y),
        RandFloat(m_VelMin.z, m_VelMax.z)
    );
    p.MaxLife = RandFloat(m_MinLife, m_MaxLife);
    p.Lifetime = p.MaxLife;
    p.Color = m_StartColor;
    p.Size = m_StartSize;
}

void ParticleEmitter::Update(float deltaTime) {
    if (m_Emitting) {
        m_EmitAccum += m_EmissionRate * deltaTime;
        while (m_EmitAccum >= 1.0f) {
            SpawnParticle();
            m_EmitAccum -= 1.0f;
        }
    }

    for (auto& p : m_Particles) {
        if (!p.Active) continue;
        p.Lifetime -= deltaTime;
        if (p.Lifetime <= 0.0f) { p.Active = false; continue; }

        float t = 1.0f - (p.Lifetime / p.MaxLife);
        p.Position += p.Velocity * deltaTime;
        p.Velocity.y += m_Gravity * deltaTime;
        p.Color = glm::mix(m_StartColor, m_EndColor, t);
        p.Size = glm::mix(m_StartSize, m_EndSize, t);
    }
}

void ParticleEmitter::Render(const Camera& camera,
    Shader& shader) const {
    float dist = glm::length(camera.GetPosition() - m_Position);
    if (dist > 200.0f) return;

    std::vector<ParticleVertex> verts;
    verts.reserve(m_Particles.size());
    for (const auto& p : m_Particles) {
        if (!p.Active) continue;
        verts.push_back({ p.Position, p.Color, p.Size });
    }
    if (verts.empty()) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    shader.Use();
    shader.SetUniformMat4("u_View",
        glm::value_ptr(camera.GetViewMatrix()));
    shader.SetUniformMat4("u_Projection",
        glm::value_ptr(camera.GetProjectionMatrix()));

    glm::mat4 view = camera.GetViewMatrix();
    glm::vec3 camRight = { view[0][0], view[1][0], view[2][0] };
    glm::vec3 camUp = { view[0][1], view[1][1], view[2][1] };
    shader.SetUniform3f("u_CamRight", camRight);
    shader.SetUniform3f("u_CamUp", camUp);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        verts.size() * sizeof(ParticleVertex),
        verts.data());

    glDrawArrays(GL_POINTS, 0, (GLsizei)verts.size());

    glBindVertexArray(0);
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}