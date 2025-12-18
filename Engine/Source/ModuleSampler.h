#pragma once

#include "Module.h"

class ModuleSampler : public Module
{
public:
	enum Type
	{
		LINEAR_WRAP,
		POINT_WRAP,
		LINEAR_CLAMP,
		POINT_CLAMP,
		COUNT
	};
public:
	ModuleSampler(ModuleD3D12* d3D12);

	bool init();
	bool CreateDefaultSamplers();

	D3D12_CPU_DESCRIPTOR_HANDLE getCPUHandle(int index) const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(samplerHeap->GetCPUDescriptorHandleForHeapStart(), index, samplerDescriptorSize); }
	D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle(int index) const { return CD3DX12_GPU_DESCRIPTOR_HANDLE(samplerHeap->GetGPUDescriptorHandleForHeapStart(), index, samplerDescriptorSize); }

	ID3D12DescriptorHeap* getHeap() const { return samplerHeap.Get(); }

private:
	ID3D12Device* device;
	ModuleD3D12* d3d12;

	ComPtr<ID3D12DescriptorHeap> samplerHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = { 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE gpuStart = { 0 };

	UINT samplerDescriptorSize = 0;
	UINT samplerCount = 0;
};