#include "Globals.h"
#include "ModuleResources.h"
#include "ModuleD3D12.h"

#include "DirectXTex.h"

struct UploadBuffer
{
	ComPtr<ID3D12Resource> resource;
	char* mappedData;
};

ModuleResources::ModuleResources(ModuleD3D12* d3D12) : d3d12(d3D12)
{
}

bool ModuleResources::init()
{
	device = d3d12->getDevice();
	if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) return false;
	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)))) return false;
	if (FAILED(commandList->Close())) return false;
	if (FAILED(commandList->Reset(commandAllocator, nullptr))) return false;
	commandQueue = d3d12->getCommandQueue();
	/*createUploadBuffer();
	createDefaultBuffer();*/
	return true;
}

ComPtr<ID3D12Resource> ModuleResources::createUploadBuffer(const void* data, size_t size, const char* name)
{
	ComPtr<ID3D12Resource> buffer;
	// 1. Describe the buffer
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	// 2. Specify UPLOAD heap properties
	CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
	// 3. Create the resource
	device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));

	// Map the buffer: get a CPU pointer to its memory
	BYTE* pData = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We won't read from it, so range is (0,0)
	buffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));

	// Copy our application data into the buffer
	memcpy(pData, data, size);
	// Unmap the buffer (invalidate the pointer)
	buffer->Unmap(0, nullptr);

	return buffer;
}

UploadBuffer ModuleResources::createMappedUploadBuffer(size_t size, const char* name)
{
	ComPtr<ID3D12Resource> buffer;
	// 1. Describe the buffer
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	// 2. Specify UPLOAD heap properties
	CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
	// 3. Create the resource
	device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));

	// Map the buffer: get a CPU pointer to its memory
	BYTE* pData = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We won't read from it, so range is (0,0)
	buffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));

	UploadBuffer uploadBuffer;

	uploadBuffer.resource = buffer;
	uploadBuffer.mappedData = reinterpret_cast<char*>(pData);

	return uploadBuffer;
}

ComPtr<ID3D12Resource> ModuleResources::createDefaultBuffer(const void* data, size_t size, const char* name)
{
	ComPtr<ID3D12Resource> buffer;
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&buffer));

	ComPtr<ID3D12Resource> stagingBuffer = createUploadBuffer(data, size, name);

	commandList->CopyResource(buffer.Get(), stagingBuffer.Get());
	
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();

	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(std::size(ppCommandLists), ppCommandLists);

	// Esperamos a que la GPU acabe de copiar para poder borrar el Upload Buffer
	d3d12->waitForGPU();

	commandAllocator->Reset();
	commandList->Reset(commandAllocator, nullptr);

	//borrar uploadbuffer
	stagingBuffer.Reset();

	return buffer;
}

ComPtr<ID3D12Resource> ModuleResources::createDepthStencil(size_t width, size_t height, const char* name)
{
	ComPtr<ID3D12Resource> texture;
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	//CORREGIR: no podemos hacer device.get() pq aun no está inicializado este modulo. o hacemos un post init en moduled3d12 y llamamos a createdepthstencil o usamos el app->...
	//device.Get()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&texture));
	app->getD3D12()->getDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&texture));
	return texture;
}

ComPtr<ID3D12Resource> ModuleResources::createTextureFromFile(const std::filesystem::path& path)
{
	DirectX::ScratchImage image;
	const wchar_t* filePath = path.c_str();
	if (FAILED(LoadFromDDSFile(filePath, DDS_FLAGS_NONE, nullptr, image)))
	{
		if (FAILED(LoadFromTGAFile(filePath, nullptr, image)))
		{
			if(FAILED(LoadFromWICFile(filePath, WIC_FLAGS_NONE, nullptr, image))) return nullptr;
		}
	}

	const DirectX::TexMetadata metaData = image.GetMetadata();

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(metaData.format, UINT64(metaData.width), UINT(metaData.height), UINT16(metaData.arraySize), UINT16(metaData.mipLevels));

	CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);
	ComPtr<ID3D12Resource> texture;
	device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&texture));

	UINT64 size = GetRequiredIntermediateSize(texture.Get(), 0, image.GetImageCount());

	ComPtr<ID3D12Resource> staging;

	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&staging));

	std::vector<D3D12_SUBRESOURCE_DATA> subData;
	subData.reserve(image.GetImageCount());
	// Note we are iteration over mipLevels of each array item to respect Subresource index order
	for (size_t item = 0; item < metaData.arraySize; ++item)
	{
		for (size_t level = 0; level < metaData.mipLevels; ++level)
		{
			const DirectX::Image* subImg = image.GetImage(level, item, 0);
			D3D12_SUBRESOURCE_DATA data = { subImg->pixels, subImg->rowPitch, subImg->slicePitch };
			subData.push_back(data);
		}
	}
	UpdateSubresources(commandList, texture.Get(), staging.Get(), 0, 0, UINT(image.GetImageCount()), subData.data());

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();

	ID3D12CommandList* ppCommandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(std::size(ppCommandLists), ppCommandLists);

	// Esperamos a que la GPU acabe de copiar para poder borrar el Upload Buffer
	d3d12->waitForGPU();

	commandAllocator->Reset();
	commandList->Reset(commandAllocator, nullptr);

	staging.Reset();

	//texture->SetName(std::wstring(name, name + strlen(name)).c_str());
	return texture;
}

