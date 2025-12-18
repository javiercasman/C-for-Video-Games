#include "Globals.h"
#include "ModuleSampler.h"
#include "ModuleD3D12.h"

ModuleSampler::ModuleSampler(ModuleD3D12* d3D12) : d3d12(d3D12)
{

}


bool ModuleSampler::init() 
{
	device = d3d12->getDevice();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = COUNT;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&samplerHeap)); // Create Sampler Descriptor Heap

	cpuStart = samplerHeap->GetCPUDescriptorHandleForHeapStart();
	gpuStart = samplerHeap->GetGPUDescriptorHandleForHeapStart();

	samplerDescriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);

	CreateDefaultSamplers(); // Create default samplers (e.g., Linear Wrap, Point Clamp)

	return true;
}

bool ModuleSampler::CreateDefaultSamplers() 
{
	D3D12_SAMPLER_DESC samplers[] = {
		// 1. Linear Wrap Sampler
		{
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // Linear filtering for min, mag, and mip
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // Wrap addressing mode
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0.0f, // Mip LOD bias
			1, // Max anisotropy
			D3D12_COMPARISON_FUNC_ALWAYS, // No comparison
			{0.0f, 0.0f, 0.0f, 0.0f}, // Border color (not used)
			0.0f, D3D12_FLOAT32_MAX // Min and Max LOD
		},
		// 2. Point Clamp Sampler
		{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0, 16, D3D12_COMPARISON_FUNC_NONE,
			{0.0f, 0.0f, 0.0f, 0.0f},
			0.0f, D3D12_FLOAT32_MAX
		},

		{
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0, 16, D3D12_COMPARISON_FUNC_NONE,
			{0.0f, 0.0f, 0.0f, 0.0f},
			0.0f, D3D12_FLOAT32_MAX
		},

		{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0, 16, D3D12_COMPARISON_FUNC_NONE,
			{0.0f, 0.0f, 0.0f, 0.0f},
			0.0f, D3D12_FLOAT32_MAX
		}
	};
	for (int i = 0; i < COUNT; ++i) 
	{
		device->CreateSampler(&samplers[i], getCPUHandle(i));
	}

	return true;
}