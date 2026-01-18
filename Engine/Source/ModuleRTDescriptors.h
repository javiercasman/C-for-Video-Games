#pragma once

#include "Module.h"
#include "HandleManager.h"

class ModuleRTDescriptors : public Module
{
public:
	ModuleRTDescriptors(ModuleD3D12* d3d12);

	bool init();
	void createRTV(ID3D12Resource* texture, UINT handle);
	UINT allocDescriptor();
	//void reset();

	void releaseRT(UINT handle);

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT handle) const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, rtHandles.indexFromHandle(handle), rtvDescriptorIncrementSize); };

	ID3D12DescriptorHeap* getHeap() const { return rtvHeap.Get(); }

private:
	enum { MaxRTVDescriptors = 256 };
	ID3D12Device* device;
	ModuleD3D12* d3d12;

	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	UINT rtvCount = 0;

	HandleManager<MaxRTVDescriptors> rtHandles;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = { 0 }; //heap start

	UINT rtvDescriptorIncrementSize = 0;
};