#pragma once

#include "Module.h"

class ModuleRingBuffer
{
public:
	bool init();
	D3D12_GPU_VIRTUAL_ADDRESS AllocBuffer(const void* data, size_t size);
	void preRender();
private:
	char* ringBufferData;
	ComPtr<ID3D12Resource> ringBuffer;
	size_t bufferSize;
	size_t totalAllocatedSize;

	UINT head;
	UINT tail;
}