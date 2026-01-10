#pragma once

#include "Module.h"

class ModuleExercise2 : public Module
{
public:
	ModuleExercise2(ModuleD3D12* d3D12);
	bool init();
	void render();
	void postRender();

	bool isExercise2() const { return isCurrentExercise; }
	void setIsExercise2(const bool isExercise) { isCurrentExercise = isExercise; }

private:
	ModuleD3D12* d3d12;
	ID3D12Device5* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandQueue* commandQueue;
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3D12RootSignature> rootSignature;
	
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	bool createVertexBuffer();
	bool createRootSignature();
	bool createPSO();

	bool isCurrentExercise = false;
};