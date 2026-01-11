#pragma once

#include "Module.h"

class ModuleRTDescriptors : public Module
{
public:
	ModuleRTDescriptors(ModuleD3D12* d3d12);

	bool init();
	void createRTV(ID3D12Resource* texture, UINT rtvIndex);
	UINT allocDescriptor();
	void reset();

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT index) const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, index, rtvDescriptorIncrementSize); };

	ID3D12DescriptorHeap* getHeap() const { return rtvHeap.Get(); }

private:
	ID3D12Device* device;
	ModuleD3D12* d3d12;

	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	UINT rtvCount = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = { 0 };

	UINT rtvDescriptorIncrementSize = 0;
};