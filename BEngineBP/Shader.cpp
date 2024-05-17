#include "Shader.h"
#include <assert.h>

namespace raw_shaders {
	namespace default_s {
		#include "shader/default/default_vertex.h"
		#include "shader/default/default_pixel.h"
	}
}

using namespace BEngine;

// Just the Class to Save them

void Shader::Init(ID3D11Device1* device)
{
}

void Shader::SetContext()
{
}

void Shader::FillBuffers()
{
}

// The Default Shader for all Objects
BEngine::DefaultShader BEngine::defaultShader = {};

void DefaultShader::Init(ID3D11Device1* device)
{
	HRESULT hRes = device->CreateVertexShader(&raw_shaders::default_s::default_vertex, sizeof(raw_shaders::default_s::default_vertex), nullptr, &this->vertexShader);
	if (FAILED(hRes))
		assert("Default Vertex Shader failed to Load!");

	hRes = device->CreatePixelShader(&raw_shaders::default_s::default_pixel, sizeof(raw_shaders::default_s::default_pixel), nullptr, &this->pixelShader);
	if (FAILED(hRes))
		assert("Default Pixel Shader failed to Load!");
}

void DefaultShader::SetContext()
{
}

void DefaultShader::FillBuffers()
{
}

void BEngine::LoadAdvancedShaders(ID3D11Device1* d3d11Device) {

	defaultShader.Init(d3d11Device);

}
