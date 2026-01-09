#pragma once

#include "Module.h"
#include "ModuleSampler.h"

class DebugDrawPass;
class Model;

class ModuleExercise5 : public Module
{
public:
	ModuleExercise5(ModuleD3D12* d3D12, ModuleCamera* Camera);
	bool init();
	void render();
	void postRender();

	void setSampler(const ModuleSampler::Type type);
	ModuleSampler::Type getSamplerType() const { return samplerType; }

	void setShowAxis(const bool show) { showAxis = show; }
	void setShowGrid(const bool show) { showGrid = show; }
	void setShowGuizmo(const bool show) { showGuizmo = show; }

	Model* getModel() const { return model; }
private:
	ModuleD3D12* d3d12;
	ModuleCamera* camera;
	ModuleShaderDescriptors* shaderDescriptors;
	ModuleSampler* samplers;
	ModuleResources* resources;

	Model* model;

	ComPtr<ID3D12Resource> textureDog;
	UINT dogDescriptorIndex = 0;

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
	ModuleSampler::Type samplerType = ModuleSampler::LINEAR_WRAP;

	bool showAxis = true;
	bool showGrid = true;
	bool showGuizmo = true;

	bool createVertexBuffer();
	bool createRootSignature();
	bool createPSO();
	bool loadModel();
};