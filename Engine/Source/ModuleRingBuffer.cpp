#include "Globals.h"
#include "ModuleRingBuffer.h"

#include "ModuleD3D12.h"
#include "ModuleResources.h"

#define RING_BUFFER_SIZE (1024 * 1024 * 10) //10 MB

ModuleRingBuffer::ModuleRingBuffer(ModuleD3D12* d3D12, ModuleResources* moduleResources) : d3d12(d3D12), resources(moduleResources)
{
}

bool ModuleRingBuffer::init()
{
	bufferSize = alignUp(RING_BUFFER_SIZE, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	ringBuffer, ringBufferData = resources->createMappedUploadBuffer(bufferSize, "Ring Buffer"); //10 MB, permanently mapped porque data == nullptr

	head = 0;
	tail = 0;

    return true;
}

D3D12_GPU_VIRTUAL_ADDRESS ModuleRingBuffer::AllocBuffer(const void* data, size_t size)
{
	if (tail < head)
	{
		size_t freeSpace = bufferSize - head;
		if (freeSpace >= size)
		{
			//allocate
			memcpy(ringBufferData+head, data, size);
			D3D12_GPU_VIRTUAL_ADDRESS address = ringBuffer->GetGPUVirtualAddress() + head;
			head += size;
			totalAllocatedSize += size;

			return address;
		}
		else
		{
			head = 0;
			totalAllocatedSize += freeSpace;
			//probaremos en el siguiente if
		}
	}

	if (tail >= head)
	{
		if (size < tail - head)
		{
			//allocate
			memcpy(ringBufferData + head, data, size);
			D3D12_GPU_VIRTUAL_ADDRESS address = ringBuffer->GetGPUVirtualAddress() + head;
			head += size;
			totalAllocatedSize += size;
			
			return address;
		}
	}

	if (tail == head)
	{
		if (totalAllocatedSize == 0)
		{
			//buffer empty. esto se puede hacer en el if anterior en verdad
			memcpy(ringBufferData + head, data, size);
			D3D12_GPU_VIRTUAL_ADDRESS address = ringBuffer->GetGPUVirtualAddress() + head;
			head += size;
			totalAllocatedSize += size;
			return address;
		}
		else
		{
			//buffer full
			return 0;
		}
	}
	return 0;
}

void ModuleRingBuffer::preRender()
{
	currentFrame = d3d12->getCurrentBackBufferIndex();

	tail = (tail + allocatedThisFrame[currentFrame]) % bufferSize;
	totalAllocatedize -= allocatedThisFrame[currentFrame];

	allocatedThisFrame[currentFrame] = 0;
}