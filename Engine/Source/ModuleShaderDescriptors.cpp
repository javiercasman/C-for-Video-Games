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

void ModuleShaderDescriptors::createSRV(ID3D12Resource* texture, UINT srvIndex)
{
	device->CreateShaderResourceView(texture, nullptr, getCPUHandle(srvIndex));
}

UINT ModuleShaderDescriptors::allocDescriptor()
{
	return srvCount++;
}

void ModuleShaderDescriptors::reset()
{
	srvCount = 0;
}
