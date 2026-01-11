#pragma once

#include "Module.h"

class ModuleDSDescriptors : public Module
{
public:
	ModuleDSDescriptors(ModuleD3D12* d3d12);

	bool init();
	void createDSV(ID3D12Resource* texture, UINT dsvIndex);
	UINT allocDescriptor();
	void reset();

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT index) const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, index, dsvDescriptorIncrementSize); };

	ID3D12DescriptorHeap* getHeap() const { return dsvHeap.Get(); }

private:
	ID3D12Device* device;
	ModuleD3D12* d3d12;

	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	UINT dsvCount = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = { 0 };

	UINT dsvDescriptorIncrementSize = 0;
};