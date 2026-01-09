#include "Globals.h"
#include "ModuleRingBuffer.h"

#include "ModuleResources.h"

#define RING_BUFFER_SIZE (1024 * 1024 * 10) //10 MB

bool ModuleRingBuffer::init()
{
	bufferSize = alignUp(RING_BUFFER_SIZE, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	ringBuffer = app->getResources()->createUploadBuffer(nullptr, bufferSize, "Ring Buffer"); //10 MB, permanently mapped porque data == nullptr
	
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
		}
		else
		{
			head = 0;
			//probaremos en el siguiente if
		}
	}

	if (tail >= head)
	{
		if (size <= tail - head)
		{
			//allocate
		}
	}

	else
	{

	}
    return 0;
}
