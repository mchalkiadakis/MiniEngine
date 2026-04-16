#include "ModelLoader.h"
#include "Texture.h"
#include "Core/AssetManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

std::string ModelLoader::s_Directory;

std::unique_ptr<Model> ModelLoader::Load(const std::string& filepath,
    std::shared_ptr<Shader> shader)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << "\n";
        return nullptr;
    }

    s_Directory = filepath.substr(0, filepath.find_last_of("/\\"));

    auto model = std::make_unique<Model>();

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* aimesh = scene->mMeshes[m];

        std::vector<Vertex> vertices;
        vertices.reserve(aimesh->mNumVertices);

        for (unsigned int i = 0; i < aimesh->mNumVertices; ++i) {
            Vertex v;
            v.Position = { aimesh->mVertices[i].x,
                            aimesh->mVertices[i].y,
                            aimesh->mVertices[i].z };

            v.Normal = aimesh->HasNormals()
                ? glm::vec3{ aimesh->mNormals[i].x,
                             aimesh->mNormals[i].y,
                             aimesh->mNormals[i].z }
            : glm::vec3{ 0.0f, 1.0f, 0.0f };

            v.TexCoords = aimesh->HasTextureCoords(0)
                ? glm::vec2{ aimesh->mTextureCoords[0][i].x,
                             aimesh->mTextureCoords[0][i].y }
            : glm::vec2{ 0.0f, 0.0f };

            vertices.push_back(v);
        }

        std::vector<unsigned int> indices;
        indices.reserve(aimesh->mNumFaces * 3);
        for (unsigned int i = 0; i < aimesh->mNumFaces; ++i) {
            const aiFace& face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        std::shared_ptr<Texture> texture;
        if (aimesh->mMaterialIndex >= 0) {
            aiMaterial* mat = scene->mMaterials[aimesh->mMaterialIndex];
            aiString texPath;
            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
                std::string fullPath = s_Directory + "/" + texPath.C_Str();
                texture = AssetManager::Get().LoadTexture(fullPath);
            }
        }

        if (!texture)
            std::cerr << "Warning: no diffuse texture for mesh " << m << "\n";

        model->AddSubMesh(
            std::make_unique<Mesh>(vertices, indices),
            std::make_unique<Material>(shader, texture)
        );
    }

    return model;
}