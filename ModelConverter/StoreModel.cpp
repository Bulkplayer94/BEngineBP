#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct VertexData {
    float pos[3];
    float uv[2];
    float norm[3];
};

struct IndiceData {
    unsigned int data[3];
};

int main()
{
    using namespace Assimp;

    Importer imp;
    const aiScene* scene = imp.ReadFile("data/gm_bigcity.obj", aiProcessPreset_TargetRealtime_Quality);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error: " << imp.GetErrorString() << std::endl;
        return -1;
    }
    
    std::ofstream file("bigcity.bmodel", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open output file." << std::endl;
        return -1;
    }

    for (unsigned int sceneIter = 0; sceneIter < scene->mNumMeshes; ++sceneIter) {
        const aiMesh* mesh = scene->mMeshes[sceneIter];

        std::vector<VertexData> vtxDat;
        for (unsigned int vtxIterator = 0; vtxIterator < mesh->mNumVertices; ++vtxIterator)
        {
            aiVector3D& position = mesh->mVertices[vtxIterator];
            aiVector3D uvCoord = mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][vtxIterator] : aiVector3D(0.0f, 0.0f, 0.0f);
            aiVector3D& normPos = mesh->mNormals[vtxIterator];

            vtxDat.push_back(
                {
                    { position[0], position[1], position[2] },
                    { uvCoord[0], uvCoord[1] },
                    { normPos[0], normPos[1], normPos[2] }
                }
            );
        }

        unsigned int vertexCount = vtxDat.size();
        file.write(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        file.write(reinterpret_cast<char*>(vtxDat.data()), vertexCount * sizeof(VertexData));

        std::vector<IndiceData> indDat;
        for (unsigned int indIterator = 0; indIterator < mesh->mNumFaces; ++indIterator)
        {
            const aiFace& face = mesh->mFaces[indIterator];
            indDat.push_back({ face.mIndices[0], face.mIndices[1], face.mIndices[2] });
        }

        unsigned int indexCount = indDat.size();
        file.write(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        file.write(reinterpret_cast<char*>(indDat.data()), indexCount * sizeof(IndiceData));

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            int x, y, comp;
            unsigned char* texture = stbi_load(("data/" + std::string(path.C_Str())).c_str(), &x, &y, &comp, 4);
            if (!texture) {
                std::cerr << "Failed to load texture: " << path.C_Str() << std::endl;
                continue;
            }

            file.write(reinterpret_cast<char*>(&x), sizeof(x));
            file.write(reinterpret_cast<char*>(&y), sizeof(y));
            file.write(reinterpret_cast<char*>(texture), x * y * 4);
            stbi_image_free(texture);
        }
        else {
            int x = 0, y = 0;
            file.write(reinterpret_cast<char*>(&x), sizeof(x));
            file.write(reinterpret_cast<char*>(&y), sizeof(y));
        }
    }

    return 0;
}
