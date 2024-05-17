#pragma once
#include <d3d11_1.h>


namespace BEngine {

	struct Shader
	{
		virtual void Init(ID3D11Device1* device);
		virtual void SetContext();
		virtual void FillBuffers();

		// Does the Positioning of the 3D Points
		ID3D11VertexShader* vertexShader = nullptr;

		//ID3D11HullShader* hullShader = nullptr;
		//ID3D11DomainShader* domainShader = nullptr;
		//ID3D11GeometryShader* geometryShader = nullptr;

		// Does the Coloring of the Sides of the Geometry
		ID3D11PixelShader* pixelShader = nullptr;

	};

	struct DefaultShader : Shader
	{
		void Init(ID3D11Device1* device);
		void SetContext();
		void FillBuffers();
	} extern defaultShader;

	void LoadAdvancedShaders(ID3D11Device1* d3d11Device);

}
