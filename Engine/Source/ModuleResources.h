#pragma once

#include "Module.h"
#include "ModuleD3D12.h"

class ModuleResources : public Module
{
public:
	ModuleResources(ModuleD3D12* d3D12);
	bool init();
	ComPtr<ID3D12Resource> CreateUploadBuffer(const void* data, size_t size, const char* name);
	ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* data, size_t size, const char* name);
private:
	ModuleD3D12* d3d12 = nullptr;
	ComPtr<ID3D12Device5>* device;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue>* commandQueue;

	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> stagingBuffer;
};