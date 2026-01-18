#include "Globals.h"
#include "ModuleShaderDescriptors.h"

#include "Application.h"
#include "ModuleD3D12.h"

ModuleShaderDescriptors::ModuleShaderDescriptors(ModuleD3D12* d3D12) : d3d12(d3D12)
{
}

bool ModuleShaderDescriptors::init()
{
	d3d12 = app->getD3D12();
	device = d3d12->getDevice();

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//srvHeapDesc.NumDescriptors = NUM_DESCRIPTORS * DESCRIPTORS_PER_TABLE;
	srvHeapDesc.NumDescriptors = 128; // fijo, simple
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)))) return false;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());
	srvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(srvHeapDesc.Type);

	cpuStart = srvHeap->GetCPUDescriptorHandleForHeapStart();
	gpuStart = srvHeap->GetGPUDescriptorHandleForHeapStart();

	return true;
}

void ModuleShaderDescriptors::preRender()
{
	collectGarbage();
}

void ModuleShaderDescriptors::createSRV(ID3D12Resource* texture, UINT handle)
{
	device->CreateShaderResourceView(texture, nullptr, getCPUHandle(handle));
}

UINT ModuleShaderDescriptors::createNullTexture2DSRV()
{
	_ASSERTE(srvCount < 128);

	UINT index = srvCount++;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Standard format
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	ID3D12Device5* device = app->getD3D12()->getDevice();
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(cpuStart, index, srvDescriptorIncrementSize);
	device->CreateShaderResourceView(nullptr, &srvDesc, handle);
	return index;
}

UINT ModuleShaderDescriptors::allocDescriptor()
{
	UINT handle = srvHandles.allocHandle();
	_ASSERTE(srvHandles.validHandle(handle));
	return handle;
	//return srvCount++;
}

void ModuleShaderDescriptors::reset()
{
	srvCount = 0;
}

void ModuleShaderDescriptors::deferRelease(UINT handle)
{
	if (handle != 0)
		srvHandles.deferRelease(handle, d3d12->getCurrentFrame());
}

void ModuleShaderDescriptors::collectGarbage()
{
	srvHandles.collectGarbage(d3d12->getLastCompletedFrame());
}
