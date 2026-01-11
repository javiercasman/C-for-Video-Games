#pragma once

#include "Module.h"

class ModuleRingBuffer : public Module
{
public:
	ModuleRingBuffer(ModuleD3D12* d3D12, ModuleResources* moduleResources);
	~ModuleRingBuffer();

	bool init();
	D3D12_GPU_VIRTUAL_ADDRESS AllocBuffer(const void* data, size_t size);
	void preRender();
private:
	ModuleD3D12* d3d12;
	ModuleResources* resources;

	char* ringBufferData;
	ComPtr<ID3D12Resource> ringBuffer;
	size_t bufferSize;
	size_t totalAllocatedSize;
	size_t allocatedThisFrame[2];

	UINT head;
	UINT tail;
	UINT currentBackBufferIndex;
};