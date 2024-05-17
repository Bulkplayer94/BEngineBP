#pragma once

#ifndef _3DManager_h
#define _3DManager_h

#include "shader/SHADER_defines.h"
//#include "ObjLoading.h"
#include "d3d11.h"
#include <vector>
#include "3DMaths.h"
#include "globals.h"

#include <PxPhysicsAPI.h>
#include "assimp/Importer.hpp"

using namespace physx;

struct Model {

    bool Load(const char*);

    bool LoadTexture(const char*);

    void UpdateViewMats();

    void Draw(SHADER*, float4x4*, float4x4*);

    void SetPosition(float3);

    float4x4 getModelViewProj(float4x4*, float4x4*);

    void SetSleeping(bool status);

    float scale = 1.0F;

    void SetShader(UINT);

    UINT numVertices = 0;
    UINT numIndices = 0;
    UINT stride = 0;
    UINT offset = 0;

    float3 Pos = { 1.0F, 1.0F, 1.0F };
    float rotation = 0.0F;

    float4x4 cubeModelViewMat = { 0.0F };
    float3x3 cubeNormalMats = { 0.0F };

    ID3D11Buffer* vertexBuffer = NULL;
    ID3D11Buffer* indexBuffer = NULL;

    ID3D11ShaderResourceView* textureView = NULL;

    UINT ShaderType = 0;

    PxRigidActor* physXActor = nullptr; // Füge einen PhysX-Actor hinzu

    bool isStatic = false;

    ~Model();

};

#endif // _3DManager_h