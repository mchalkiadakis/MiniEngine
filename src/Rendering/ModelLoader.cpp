#include "ModelLoader.h"
#include "Texture.h"
#include "Core/AssetManager.h"
#include "Skeleton.h"
#include "AnimationClip.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

std::string ModelLoader::s_Directory;
std::unordered_map<std::string, std::shared_ptr<Texture>> ModelLoader::s_TextureCache;

static glm::mat4 AiToGlm(const aiMatrix4x4& m) {
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

static glm::vec3 AiToGlm(const aiVector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}

static glm::quat AiToGlm(const aiQuaternion& q) {
    return glm::quat(q.w, q.x, q.y, q.z);
}

static void ExtractBoneHierarchy(aiNode* node,
    int parentIndex,
    Skeleton& skeleton,
    const std::unordered_map<std::string, BoneInfo>& boneInfoMap,
    int depth = 0)
{
    if (!node) return;
    if (depth > 64) return;
    if (skeleton.GetBoneCount() > 500) return;

    std::string nodeName = node->mName.C_Str();

    glm::mat4 offsetMatrix{ 1.0f };
    int originalId = -1;
    if (boneInfoMap.count(nodeName)) {
        offsetMatrix = boneInfoMap.at(nodeName).OffsetMatrix;
        originalId = boneInfoMap.at(nodeName).Id;
    }

    int boneIndex = skeleton.AddBone(nodeName, parentIndex, offsetMatrix);
    skeleton.GetBone(boneIndex).LocalTransform = AiToGlm(node->mTransformation);
    skeleton.GetBone(boneIndex).OriginalId = originalId;

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        ExtractBoneHierarchy(node->mChildren[i], boneIndex, skeleton,
            boneInfoMap, depth + 1);
}

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

std::unique_ptr<SkinnedModel> ModelLoader::LoadSkinned(
    const std::string& filepath,
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
        std::cerr << "Assimp error (skinned): " << importer.GetErrorString() << "\n";
        return nullptr;
    }

    s_Directory = filepath.substr(0, filepath.find_last_of("/\\"));

    std::cout << "Loading skinned model: " << filepath << "\n";
    std::cout << "Animations found: " << scene->mNumAnimations << "\n";
    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        std::cout << "  - " << scene->mAnimations[i]->mName.C_Str() << "\n";

    // --- pass 1: collect bone info ---
    std::unordered_map<std::string, BoneInfo> boneInfoMap;
    int boneCounter = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* aimesh = scene->mMeshes[m];
        for (unsigned int b = 0; b < aimesh->mNumBones; b++) {
            aiBone* bone = aimesh->mBones[b];
            std::string name = bone->mName.C_Str();
            if (!boneInfoMap.count(name)) {
                BoneInfo info;
                info.Id = boneCounter++;
                info.OffsetMatrix = AiToGlm(bone->mOffsetMatrix);
                boneInfoMap[name] = info;
            }
        }
    }

    // --- pass 2: build skinned meshes ---
    auto skinnedModel = std::make_unique<SkinnedModel>();

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* aimesh = scene->mMeshes[m];

        std::vector<SkinnedVertex> vertices(aimesh->mNumVertices);

        for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
            vertices[i].Position = AiToGlm(aimesh->mVertices[i]);

            vertices[i].Normal = aimesh->HasNormals()
                ? AiToGlm(aimesh->mNormals[i])
                : glm::vec3(0.0f, 1.0f, 0.0f);

            vertices[i].TexCoords = aimesh->HasTextureCoords(0)
                ? glm::vec2(aimesh->mTextureCoords[0][i].x,
                    aimesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);

            vertices[i].Tangent = aimesh->HasTangentsAndBitangents()
                ? AiToGlm(aimesh->mTangents[i])
                : glm::vec3(1.0f, 0.0f, 0.0f);

            for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
                vertices[i].BoneIds[j] = -1;
                vertices[i].BoneWeights[j] = 0.0f;
            }
        }

        for (unsigned int b = 0; b < aimesh->mNumBones; b++) {
            aiBone* bone = aimesh->mBones[b];
            std::string boneName = bone->mName.C_Str();
            int         boneId = boneInfoMap[boneName].Id;

            for (unsigned int w = 0; w < bone->mNumWeights; w++) {
                int   vertexId = bone->mWeights[w].mVertexId;
                float weight = bone->mWeights[w].mWeight;
                if (vertexId < (int)vertices.size())
                    vertices[vertexId].AddBoneData(boneId, weight);
            }
        }

        std::vector<unsigned int> indices;
        indices.reserve(aimesh->mNumFaces * 3);
        for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
            const aiFace& face = aimesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
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

        skinnedModel->AddSubMesh(
            std::make_unique<SkinnedMesh>(vertices, indices),
            std::make_unique<Material>(shader, texture)
        );
    }

    // --- pass 3: build skeleton ---
    
    auto skeleton = std::make_shared<Skeleton>();
    ExtractBoneHierarchy(scene->mRootNode, -1, *skeleton, boneInfoMap, 0);
    std::cout << "Total skeleton nodes: " << skeleton->GetBoneCount() << "\n";
    skinnedModel->SetSkeleton(skeleton);

    glm::mat4 globalInverse = glm::inverse(AiToGlm(scene->mRootNode->mTransformation));
    skinnedModel->SetGlobalInverseTransform(globalInverse);

    // --- pass 4: extract clips ---
    for (unsigned int a = 0; a < scene->mNumAnimations; a++) {
        aiAnimation* anim = scene->mAnimations[a];
        AnimationClip clip;
        clip.Name = anim->mName.C_Str();
        clip.Duration = (float)anim->mDuration;
        clip.TicksPerSec = (float)(anim->mTicksPerSecond != 0
            ? anim->mTicksPerSecond : 24.0);

        for (unsigned int c = 0; c < anim->mNumChannels; c++) {
            aiNodeAnim* channel = anim->mChannels[c];
            BoneChannel boneChannel;
            boneChannel.BoneName = channel->mNodeName.C_Str();

            for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
                KeyPosition kp;
                kp.Position = AiToGlm(channel->mPositionKeys[k].mValue);
                kp.Time = (float)channel->mPositionKeys[k].mTime;
                boneChannel.Positions.push_back(kp);
            }

            for (unsigned int k = 0; k < channel->mNumRotationKeys; k++) {
                KeyRotation kr;
                kr.Rotation = AiToGlm(channel->mRotationKeys[k].mValue);
                kr.Time = (float)channel->mRotationKeys[k].mTime;
                boneChannel.Rotations.push_back(kr);
            }

            for (unsigned int k = 0; k < channel->mNumScalingKeys; k++) {
                KeyScale ks;
                ks.Scale = AiToGlm(channel->mScalingKeys[k].mValue);
                ks.Time = (float)channel->mScalingKeys[k].mTime;
                boneChannel.Scales.push_back(ks);
            }

            clip.Channels[boneChannel.BoneName] = boneChannel;
        }

        skinnedModel->AddClip(clip);
        std::cout << "Loaded clip: " << clip.Name
            << " duration=" << clip.Duration << "\n";
    }

    return skinnedModel;
}