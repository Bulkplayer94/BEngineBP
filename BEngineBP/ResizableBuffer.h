#pragma once

#include <d3d11_1.h>

namespace BEngine {
	template<typename T>
	struct ResizeableBuffer {
		ResizeableBuffer() {
			dataBuffer.push_back(T());
			bufferCapacity = dataBuffer.size();
		}

		~ResizeableBuffer() {
			if (d3d11Buffer) {
				d3d11Buffer->Release();
			}
			if (d3d11ShaderResources) {
				d3d11ShaderResources->Release();
			}
		}

		std::vector<T> dataBuffer = {};
		size_t bufferCapacity = 0;
		ID3D11Buffer* d3d11Buffer = nullptr;
		ID3D11ShaderResourceView* d3d11ShaderResources = nullptr;

		void CreateBuffer();
		bool RefreshBuffer();
	};
}