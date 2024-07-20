#pragma once

#ifndef SHADER_DEFINES_H
#define SHADER_DEFINES_H

#include <string>

#include <d3d11.h>
#include "../3DMaths.h"

typedef void(*SHADER_UPDATE_BUFFER)(ID3D11DeviceContext* d3d11DeviceContext, const float4x4* ModelViewProj, const float4x4* worldMat, const float4x4* perspMat, const float4x4* viewMat);
typedef void(*SHADER_SET_CONTEXT)(ID3D11DeviceContext*, std::string*);


struct SHADER {
	std::string shaderName;

	ID3D11VertexShader** VSShader;
	ID3D11PixelShader** PSShader;
	ID3D11Buffer** constantBuffer;
	ID3D11InputLayout** inputLayout;

	SHADER_UPDATE_BUFFER sBufferFunc;
	SHADER_SET_CONTEXT sContextFunc;
};

enum E_SHADER_ {
	E_SHADER_NONE,
	E_SHADER_DEFAULT,
	E_SHADER_WIRE,
	E_SHADER_AMOUNT
};

#endif // SHADER_DEFINES_H