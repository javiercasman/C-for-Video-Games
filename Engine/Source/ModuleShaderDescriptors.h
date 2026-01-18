#pragma once

#include "Module.h"
#include "DeferredFreeHandleManager.h"

class ModuleShaderDescriptors : public Module
{
public:
	ModuleShaderDescriptors(ModuleD3D12* d3D12);

	bool init();
	void preRender();
	void createSRV(ID3D12Resource* texture, UINT handle);
	UINT createNullTexture2DSRV();
	UINT allocDescriptor();
	void reset();

	void deferRelease(UINT handle);
	void collectGarbage();

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT handle) const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, srvHandles.indexFromHandle(handle), srvDescriptorIncrementSize); };
	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle(UINT handle) const { return CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuStart, srvHandles.indexFromHandle(handle), srvDescriptorIncrementSize); };

	ID3D12DescriptorHeap* getHeap() const { return srvHeap.Get(); }

private:
	//enum { NUM_DESCRIPTORS = 4096, DESCRIPTORS_PER_TABLE = 8 };
	enum { MaxSrvDescriptors = 256 };

	ID3D12Device* device;
	ModuleD3D12* d3d12;

	ComPtr<ID3D12DescriptorHeap> srvHeap;
	//int nextFreeSlot;
	UINT srvCount = 0;

	DeferredFreeHandleManager<MaxSrvDescriptors> srvHandles;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = {0}; //?
	D3D12_GPU_DESCRIPTOR_HANDLE gpuStart = {0};

	UINT srvDescriptorIncrementSize = 0;
};
