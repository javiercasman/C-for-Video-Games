#include "Globals.h"
#include "ModuleResources.h"

ModuleResources::ModuleResources(ModuleD3D12* d3D12) : d3d12(d3D12)
{
}

bool ModuleResources::init()
{
	device = d3d12->getDevice();
	if (FAILED(device->Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) return false;
	if (FAILED(device->Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) return false;
	if (FAILED(commandList->Close())) return false;
	commandQueue = d3d12->getCommandQueue();
	/*CreateUploadBuffer();
	CreateDefaultBuffer();*/
}

ComPtr<ID3D12Resource> ModuleResources::CreateUploadBuffer(const void* data, size_t size, const char* name)
{
	// 1. Describe the buffer
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	// 2. Specify UPLOAD heap properties
	CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
	// 3. Create the resource
	device->Get()->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&stagingBuffer));

	// Map the buffer: get a CPU pointer to its memory
	BYTE* pData = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We won't read from it, so range is (0,0)
	stagingBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));
	// Copy our application data into the buffer
	memcpy(pData, data, size);
	// Unmap the buffer (invalidate the pointer)
	stagingBuffer->Unmap(0, nullptr);

	return stagingBuffer;
}

ComPtr<ID3D12Resource> ModuleResources::CreateDefaultBuffer(const void* data, size_t size, const char* name)
{
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);
	device->Get()->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&vertexBuffer));

	commandList->CopyResource(vertexBuffer.Get(), stagingBuffer.Get());
	
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();

	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->Get()->ExecuteCommandLists(std::size(ppCommandLists), ppCommandLists);

	// Esperamos a que la GPU acabe de copiar para poder borrar el Upload Buffer
	d3d12->WaitForGPU();

	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);

	//como borrar uploadbuffer?

	return vertexBuffer;
}

