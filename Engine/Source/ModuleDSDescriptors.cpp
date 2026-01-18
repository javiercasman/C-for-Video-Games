#include "Globals.h"
#include "ModuleDSDescriptors.h"

#include "ModuleD3D12.h"

ModuleDSDescriptors::ModuleDSDescriptors(ModuleD3D12* d3d12) : d3d12(d3d12)
{
}

bool ModuleDSDescriptors::init()
{
	d3d12 = app->getD3D12();
	device = d3d12->getDevice();

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NumDescriptors = 128;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)))) return false;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
	dsvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(dsvHeapDesc.Type);

	cpuStart = dsvHeap->GetCPUDescriptorHandleForHeapStart();

	return true;
}

void ModuleDSDescriptors::createDSV(ID3D12Resource* texture, UINT handle)
{
	device->CreateDepthStencilView(texture, nullptr, getCPUHandle(handle));
}

UINT ModuleDSDescriptors::allocDescriptor()
{
	UINT handle = dsHandles.allocHandle();
	_ASSERTE(dsHandles.validHandle(handle));
	return handle;
	//return dsvCount++;
}

void ModuleDSDescriptors::releaseDS(UINT handle)
{
	if (handle != 0)
		dsHandles.freeHandle(handle);
}

/*void ModuleDSDescriptors::reset()
{
	dsvCount = 0;
}*/