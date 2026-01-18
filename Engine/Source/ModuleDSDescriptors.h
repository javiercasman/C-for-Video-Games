#pragma once

#include "Module.h"
#include "HandleManager.h"

class ModuleDSDescriptors : public Module
{
public:
	ModuleDSDescriptors(ModuleD3D12* d3d12);

	bool init();
	void createDSV(ID3D12Resource* texture, UINT handle);
	UINT allocDescriptor();
	//void reset();

	void releaseDS(UINT handle);

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT handle) const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, dsHandles.indexFromHandle(handle), dsvDescriptorIncrementSize); };

	ID3D12DescriptorHeap* getHeap() const { return dsvHeap.Get(); }

private:
	enum { MaxDSVDescriptors = 128 };
	ID3D12Device* device;
	ModuleD3D12* d3d12;

	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	UINT dsvCount = 0;

	HandleManager<MaxDSVDescriptors> dsHandles;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = { 0 }; //heap start

	UINT dsvDescriptorIncrementSize = 0;
};