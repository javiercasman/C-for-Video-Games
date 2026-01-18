#pragma once

#include "Module.h"

#include <filesystem>

struct UploadBuffer;

class ModuleResources : public Module
{
public:
	ModuleResources(ModuleD3D12* d3D12);
	bool init();
	void preRender();
	ComPtr<ID3D12Resource> createUploadBuffer(const void* data, size_t size, const char* name);
	UploadBuffer createMappedUploadBuffer(size_t size, const char* name);
	ComPtr<ID3D12Resource> createDefaultBuffer(const void* data, size_t size, const char* name);
	ComPtr<ID3D12Resource> createDepthStencil(size_t width, size_t height, DXGI_FORMAT format, const float clearDepth, const uint8_t clearStencil, const char* name);
	ComPtr<ID3D12Resource> createTextureFromFile(const std::filesystem::path& path);
	ComPtr<ID3D12Resource> createRenderTargetTexture(size_t width, size_t height, DXGI_FORMAT format, const Vector4& clearColour, const char* name);

	void deferRelease(ComPtr<ID3D12Resource> resource);
private:
	ModuleD3D12* d3d12 = nullptr;
	ID3D12Device5* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandQueue* commandQueue;

	struct DeferredRelease
	{
		ComPtr<ID3D12Resource> resource;
		//ID3D12Resource* resource;
		UINT frameIndex = 0;
	};

	std::vector<DeferredRelease> m_deferredReleases;

	//ComPtr<ID3D12Resource> vertexBuffer;
	//ComPtr<ID3D12Resource> stagingBuffer;
};