#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include "Direct3DManager.h"
#include <stb_image.h>
#include <stb_image_resize2.h>
#include <stdexcept>

struct TextureAtlas {
    std::vector<ID3D11Texture2D*> m_textureList;
    ID3D11ShaderResourceView* m_textureSRV = nullptr;

    // Add a texture to the Atlas
    // Returns the position of the texture in the vector or -1 if failed
    int AppendTexture(const std::string& texturePath) {
        if (!std::filesystem::exists(texturePath))
            throw std::runtime_error("Path does not exist: " + texturePath);

        int x, y, chan;
        unsigned char* textureData = stbi_load(texturePath.c_str(), &x, &y, &chan, 4);
        if (!textureData)
            throw std::runtime_error("Failed to load texture: " + texturePath);

        std::vector<unsigned char> resizedTextureData(128 * 128 * 4);
        stbir_resize_uint8_linear(textureData, x, y, 0, resizedTextureData.data(), 128, 128, 0, stbir_pixel_layout::STBIR_RGBA);

        stbi_image_free(textureData);

        ID3D11Texture2D* texture2D = nullptr;

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = 128;
        textureDesc.Height = 128;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
        textureSubresourceData.pSysMem = resizedTextureData.data();
        textureSubresourceData.SysMemPitch = 4 * 128;

        HRESULT hRes = BEngine::direct3DManager.m_d3d11Device->CreateTexture2D(&textureDesc, &textureSubresourceData, &texture2D);
        if (FAILED(hRes)) {
            throw std::runtime_error("Failed to create texture (HRESULT: " + std::to_string(hRes) + ")");
        }

        m_textureList.push_back(texture2D);
        RefreshSRV();

        return static_cast<int>(m_textureList.size()) - 1;
    }

    void RefreshSRV() {
        RELEASE_D3D11_OBJECT(m_textureSRV);

        D3D11_SHADER_RESOURCE_VIEW_DESC texSRVDesc = {};
        texSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        texSRVDesc.Texture2DArray.ArraySize = static_cast<UINT>(m_textureList.size());
        texSRVDesc.Texture2DArray.FirstArraySlice = 0;
        texSRVDesc.Texture2DArray.MostDetailedMip = 0;
        texSRVDesc.Texture2DArray.MipLevels = 1;

        HRESULT hRes = BEngine::direct3DManager.m_d3d11Device->CreateShaderResourceView(m_textureList[0], &texSRVDesc, &m_textureSRV);
        if (FAILED(hRes)) {
            throw std::runtime_error("Failed to create SRV (HRESULT: " + std::to_string(hRes) + ")");
        }
    }

    void Cleanup() {
        for (auto& texture : m_textureList) {
            if (texture) {
                texture->Release();
            }
        }
        m_textureList.clear();

        if (m_textureSRV) {
            m_textureSRV->Release();
            m_textureSRV = nullptr;
        }
    }
};
