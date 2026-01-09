#pragma once

#include "Module.h"

class ModuleRingBuffer
{
public:
	bool init();
	D3D12_GPU_VIRTUAL_ADDRESS AllocBuffer(const void* data, size_t size);
	void preRender();
private:
	ComPtr<ID3D12Resource> ringBuffer;
	size_t bufferSize;
	UINT head;
	UINT tail;
}