#pragma once

#include "Module.h"

class ModuleShaderDescriptors : public Module
{
public:
	ModuleShaderDescriptors();

	bool init();
	void createSRV(ID3D12Resource* texture);
	UINT allocDescriptor();
	void reset();

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT index) const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, index, srvDescriptorIncrementSize); };
	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle(UINT index) const { return CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuStart, index, srvDescriptorIncrementSize); };

private:
	//enum { NUM_DESCRIPTORS = 4096, DESCRIPTORS_PER_TABLE = 8 };

	ID3D12Device* device;
	ModuleD3D12* d3d12;

	ComPtr<ID3D12DescriptorHeap> srvHeap;
	//int nextFreeSlot;
	UINT srvCount = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = {0};
	D3D12_GPU_DESCRIPTOR_HANDLE gpuStart = {0};

	UINT srvDescriptorIncrementSize = 0;
};
