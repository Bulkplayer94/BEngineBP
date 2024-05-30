#pragma once

#ifndef SHADER_DEFAULT_H
#define SHADER_DEFAULT_H

#include "../3DMaths.h"
#include "SHADER_defines.h"
#include <assert.h>
#include <d3dcompiler.h>

namespace SHADER_DEFAULT {

    static ID3D11VertexShader* VSShader;
    static ID3D11PixelShader* PSShader;
    static ID3D11Buffer* constantBuffer;
    static ID3D11InputLayout* inputLayout;
    
    struct CSBuffer {
        float4x4 modelViewProj;
    };

    inline void setContext(ID3D11DeviceContext* d3d11DeviceContext, std::string* currentShader);
    inline void updateBuffer(ID3D11DeviceContext* d3d11DeviceContext, float4x4 mdlPMat);
    inline SHADER Load(ID3D11Device* d3d11Device);
}

void SHADER_DEFAULT::setContext(ID3D11DeviceContext* d3d11DeviceContext, std::string* currentShader) {
    using namespace SHADER_DEFAULT;

    if (*currentShader != "DEFAULT") {
        *currentShader = "DEFAULT";

        d3d11DeviceContext->IASetInputLayout(inputLayout);
        d3d11DeviceContext->VSSetShader(VSShader, nullptr, 0);
        d3d11DeviceContext->PSSetShader(PSShader, nullptr, 0);
        d3d11DeviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);
        d3d11DeviceContext->PSSetConstantBuffers(0, 1, &constantBuffer);

    }

}

void SHADER_DEFAULT::updateBuffer(ID3D11DeviceContext* d3d11DeviceContext, float4x4 ModelViewProj) {
    // ID3D11DeviceContext* d3d11DeviceContext = *IN_CONTEXT;
    {
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        d3d11DeviceContext->Map(SHADER_DEFAULT::constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
        SHADER_DEFAULT::CSBuffer* constants = (SHADER_DEFAULT::CSBuffer*)(mappedSubresource.pData);
        constants->modelViewProj = ModelViewProj;
        d3d11DeviceContext->Unmap(SHADER_DEFAULT::constantBuffer, 0);
    }
}

SHADER SHADER_DEFAULT::Load(ID3D11Device* d3d11Device) {
    //ID3D11Device* d3d11Device = INd3dDevice;

    SHADER SDefault;
    SDefault.sBufferFunc = &updateBuffer;
    SDefault.sContextFunc = &setContext;
    SDefault.VSShader = &VSShader;
    SDefault.PSShader = &PSShader;
    SDefault.constantBuffer = &constantBuffer;
    SDefault.inputLayout = &inputLayout;

    UINT shaderCompileFlags = 0;
    // Compiling with this flag allows debugging shaders with Visual Studio
#if defined(_DEBUG)
    shaderCompileFlags |= D3DCOMPILE_DEBUG;
#endif

    // Create Vertex Shader
    ID3DBlob* vsBlob;
    // ID3D11VertexShader* vertexShader;
    {
        ID3DBlob* shaderCompileErrorsBlob;
        HRESULT hResult = D3DCompileFromFile(L"shader\\default.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", shaderCompileFlags, 0, &vsBlob, &shaderCompileErrorsBlob);
        if (FAILED(hResult))
        {
            const char* errorString = NULL;
            if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                errorString = "Could not compile shader; file not found";
            else if (shaderCompileErrorsBlob) {
                errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                shaderCompileErrorsBlob->Release();
            }
            MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
            // return 1;
        }

        hResult = d3d11Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &VSShader);
        assert(SUCCEEDED(hResult));
    }

    // Create Pixel Shader
    // ID3D11PixelShader* pixelShader;
    {
        ID3DBlob* psBlob;
        ID3DBlob* shaderCompileErrorsBlob;
        HRESULT hResult = D3DCompileFromFile(L"shader\\default.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", shaderCompileFlags, 0, &psBlob, &shaderCompileErrorsBlob);
        if (FAILED(hResult))
        {
            const char* errorString = NULL;
            if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                errorString = "Could not compile shader; file not found";
            else if (shaderCompileErrorsBlob) {
                errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                shaderCompileErrorsBlob->Release();
            }
            MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
            // return 1;
        }

        hResult = d3d11Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &PSShader);
        assert(SUCCEEDED(hResult));
        psBlob->Release();
    }

    // Create Input Layout
    //ID3D11InputLayout* inputLayout;
    {

        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POS",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                          D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEX",  0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(float) * 3,          D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * (3 + 2),    D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        HRESULT hResult = d3d11Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
        assert(SUCCEEDED(hResult));
        vsBlob->Release();
    }

    // ID3D11Buffer* constantBuffer;
    {
        D3D11_BUFFER_DESC constantBufferDesc = {};
        // ByteWidth must be a multiple of 16, per the docs
        constantBufferDesc.ByteWidth = sizeof(CSBuffer) + 0xf & 0xfffffff0;
        constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hResult = d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
        assert(SUCCEEDED(hResult));
    }

    SDefault.shaderName = "DEFAULT";

    return SDefault;
}
#endif // SHADER_DEFAULT_H