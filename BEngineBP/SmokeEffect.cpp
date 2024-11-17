#include "pch.h"
#include "SmokeEffect.h"
#include "Direct3DManager.h"

#include "CCamera.h"

#include "data/compiled_shader/SmokeVS.h"
#include "data/compiled_shader/SmokePS.h"

#include <stb_image.h>

using namespace BEngine;
using namespace DirectX;


bool SmokeEffect::Initialize()
{
    HRESULT hRes;
    hRes = direct3DManager.m_d3d11Device->CreateVertexShader(SmokeVS, ARRAYSIZE(SmokeVS), nullptr, &m_vertexShader);
    if (FAILED(hRes)) {
        return false;
    }

    hRes = direct3DManager.m_d3d11Device->CreatePixelShader(SmokePS, ARRAYSIZE(SmokePS), nullptr, &m_pixelShader);
    if (FAILED(hRes)) {
        return false;
    }

    constexpr D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hRes = direct3DManager.m_d3d11Device->CreateInputLayout(layout, ARRAYSIZE(layout), SmokeVS, ARRAYSIZE(SmokeVS), &m_inputLayout);
    if (FAILED(hRes)) {
        return false;
    }

    Vertex vertices[] = {
        {XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)},
        {XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)},
        {XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)},
        {XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)},
    };

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;

    D3D11_BUFFER_DESC bufferDesc = {};
    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    hRes = direct3DManager.m_d3d11Device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(hRes)) {
        return false;
    }

    UINT indices[] = {
        0, 1, 2,
        2, 1, 3 
    };

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;

    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.ByteWidth = sizeof(UINT) * ARRAYSIZE(indices);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    
    hRes = direct3DManager.m_d3d11Device->CreateBuffer(&bufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(hRes)) {
        return false;
    }

    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    bufferDesc.ByteWidth = (sizeof(CBuffer) + 15) & ~15; // Auf 16 Bytes aufrunden
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hRes = direct3DManager.m_d3d11Device->CreateBuffer(&bufferDesc, nullptr, &m_matrixBuffer);
    if (FAILED(hRes)) {
        return false;
    }

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;

    D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
    rtBlendDesc.BlendEnable = TRUE;
    rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
    rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
    rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    blendDesc.RenderTarget[0] = rtBlendDesc;

    hRes = direct3DManager.m_d3d11Device->CreateBlendState(&blendDesc, &m_blendState);
    if (FAILED(hRes)) {
        return false;
    }

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.BorderColor[0] = 0.0f; // Red
    samplerDesc.BorderColor[1] = 0.0f; // Green
    samplerDesc.BorderColor[2] = 0.0f; // Blue
    samplerDesc.BorderColor[3] = 0.0f; // Alpha
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hRes = direct3DManager.m_d3d11Device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(hRes)) {
        return false;
    }

    m_gradientTexture = LoadTexture("data\\textures\\light_bulb.png");
}

ID3D11ShaderResourceView* BEngine::SmokeEffect::LoadTexture(std::string filePath)
{
    int texWidth, texHeight, texNumChannels = 0;

    unsigned char* textureBytes = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texNumChannels, 4);
    if (textureBytes == nullptr)
        return nullptr;

    int textureBytesPerRow = 4 * texWidth;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = texWidth;
    textureDesc.Height = texHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
    textureSubresourceData.pSysMem = textureBytes;
    textureSubresourceData.SysMemPitch = textureBytesPerRow;

    ID3D11Texture2D* texture;
    if (FAILED(BEngine::direct3DManager.m_d3d11Device->CreateTexture2D(&textureDesc, &textureSubresourceData, &texture))) {
        return nullptr;
    }

    ID3D11ShaderResourceView* textureView;
    if (FAILED(BEngine::direct3DManager.m_d3d11Device->CreateShaderResourceView(texture, nullptr, &textureView))) {
        return nullptr;        
    }
    texture->Release();

    free(textureBytes);

    return textureView;
}

void BEngine::SmokeEffect::Draw(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projMatrix)
{
    using namespace BEngine;

    ID3D11DeviceContext1* ctx = direct3DManager.m_d3d11DeviceContext;

    XMVECTOR smokePos = XMVectorSet(-100.0F, -100.0F, 40.0F, 1.0F);

    XMVECTOR camPos = XMLoadFloat3(&playerCamera.position);

    XMVECTOR lookDir = smokePos - camPos;
    lookDir = XMVector3Normalize(lookDir);

    float pitch = asinf(XMVectorGetY(lookDir)); 
    float yaw = atan2f(XMVectorGetX(lookDir), XMVectorGetZ(lookDir));

    XMMATRIX rotationX = XMMatrixRotationX(-pitch);
    XMMATRIX rotationY = XMMatrixRotationY(yaw);

    XMMATRIX rotationMatrix = XMMatrixMultiply(rotationX, rotationY);

    XMMATRIX translationMatrix = XMMatrixTranslationFromVector(smokePos);
    XMMATRIX scalingMatrix = XMMatrixScaling(2.0F, 2.0F, 2.0F);

    XMMATRIX worldMatrix = XMMatrixMultiply(scalingMatrix, rotationMatrix);
    worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);

    worldMatrix = XMMatrixTranspose(worldMatrix);

    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    ctx->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    {
        CBuffer* buffer = (CBuffer*)mappedSubresource.pData;
        buffer->projMatrix = projMatrix;
        buffer->viewMatrix = viewMatrix;
        XMStoreFloat4x4(&buffer->worldMatrix, worldMatrix);
    }
    ctx->Unmap(m_matrixBuffer, 0);


    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    ctx->IASetInputLayout(m_inputLayout);
    ctx->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    ctx->VSSetShader(m_vertexShader, nullptr, 0);
    ctx->VSSetConstantBuffers(3, 1, &m_matrixBuffer);

    ctx->PSSetShader(m_pixelShader, nullptr, 0);
    ctx->PSSetSamplers(0, 1, &m_samplerState);
    ctx->PSSetShaderResources(0, 1, &m_gradientTexture);

    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    ctx->OMSetBlendState(m_blendState, blendFactor, 0xffffffff);

    ctx->DrawIndexed(6, 0, 0);

    ctx->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void BEngine::SmokeEffect::Cleanup()
{
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
    if (m_inputLayout) m_inputLayout->Release();
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
    if (m_matrixBuffer) m_matrixBuffer->Release();
    if (m_blendState) m_blendState->Release();
    if (m_samplerState) m_samplerState->Release();
    if (m_gradientTexture) m_gradientTexture->Release();

    m_vertexShader = nullptr;
    m_indexBuffer = nullptr;
    m_pixelShader = nullptr;
    m_inputLayout = nullptr;
    m_vertexBuffer = nullptr;
    m_matrixBuffer = nullptr;
    m_blendState = nullptr;
    m_samplerState = nullptr;
    m_gradientTexture = nullptr;
}

