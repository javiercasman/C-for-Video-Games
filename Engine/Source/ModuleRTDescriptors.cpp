#include "Globals.h"
#include "ModuleRTDescriptors.h"

#include "ModuleD3D12.h"

ModuleRTDescriptors::ModuleRTDescriptors(ModuleD3D12* d3d12) : d3d12(d3d12)
{
}

bool ModuleRTDescriptors::init()
{
	d3d12 = app->getD3D12();
	device = d3d12->getDevice();

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = 128;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) return false;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	rtvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

	cpuStart = rtvHeap->GetCPUDescriptorHandleForHeapStart();

	return true;
}

void ModuleRTDescriptors::createRTV(ID3D12Resource* texture, UINT handle)
{
	device->CreateRenderTargetView(texture, nullptr, getCPUHandle(handle));
}

UINT ModuleRTDescriptors::allocDescriptor()
{
	UINT handle = rtHandles.allocHandle();
	_ASSERTE(rtHandles.validHandle(handle));
	return handle;
	//return rtvCount++;
}

/*void ModuleRTDescriptors::reset()
{
	rtvCount = 0;
}*/

void ModuleRTDescriptors::releaseRT(UINT handle)
{
	if(handle != 0)
		rtHandles.freeHandle(handle);
}
