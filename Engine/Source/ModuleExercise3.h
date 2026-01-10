#pragma once

#include "Module.h"

class DebugDrawPass;

class ModuleExercise3 : public Module
{
public:
	ModuleExercise3(ModuleD3D12* d3D12, ModuleCamera* Camera);
	bool init();
	void render();
	void postRender();

	bool isExercise3() const { return isCurrentExercise; }
	void setIsExercise3(const bool isExercise) { isCurrentExercise = isExercise; }

private:
	ModuleD3D12* d3d12;
	ModuleCamera* camera;

	ComPtr<ID3D12Resource> vertexBuffer;
	//ComPtr<ID3D12Resource> depthStencil;
	ComPtr<ID3D12PipelineState> pso;
	ID3D12Device5* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;
	ComPtr<ID3D12RootSignature> rootSignature;
	ID3D12CommandQueue* commandQueue;
	//ComPtr<ID3D12DescriptorHeap> dsDescriptorHeap;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	unsigned windowWidth, windowHeight;
	DebugDrawPass* debugDraw;

	bool createVertexBuffer();
	bool createRootSignature();
	bool createPSO();

	bool isCurrentExercise = false;
};
