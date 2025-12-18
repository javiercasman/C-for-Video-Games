#pragma once

#include "Module.h"

#include <filesystem>

class ModuleResources : public Module
{
public:
	ModuleResources(ModuleD3D12* d3D12);
	bool init();
	ComPtr<ID3D12Resource> createUploadBuffer(const void* data, size_t size, const char* name);
	ComPtr<ID3D12Resource> createDefaultBuffer(const void* data, size_t size, const char* name);
	ComPtr<ID3D12Resource> createDepthStencil(size_t width, size_t height, const char* name);
	ComPtr<ID3D12Resource> createTextureFromFile(const std::filesystem::path& path);
private:
	ModuleD3D12* d3d12 = nullptr;
	ID3D12Device5* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandQueue* commandQueue;

	//ComPtr<ID3D12Resource> vertexBuffer;
	//ComPtr<ID3D12Resource> stagingBuffer;
};