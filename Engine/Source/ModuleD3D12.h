#pragma once

#include "Module.h"
#include "ImGuiPass.h"
#include <dxgi1_6.h>

class ModuleD3D12 : public Module
{
public:
	ModuleD3D12(HWND wnd);

	bool init();
	void update();
	void preRender();
	void postRender();
	void render();
	bool resize();
	void waitForGPU();
	bool cleanUp();
	void getWindowSize(UINT& width, UINT& height) const;

	ID3D12Device5* getDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* getCommandList() const { return commandList.Get(); }
	ID3D12CommandAllocator* getCommandAllocator() const { return commandAllocators[currentBackBufferIndex].Get(); }
	ID3D12CommandQueue* getCommandQueue() const { return commandQueue.Get(); }
	ID3D12Resource* getCurrentBackBuffer() const { return renderTargets[currentBackBufferIndex].Get(); }
	UINT getCurrentBackBufferIndex() const { return currentBackBufferIndex; }
	CD3DX12_CPU_DESCRIPTOR_HANDLE getCurrentRtv() const { CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), currentBackBufferIndex, rtvDescriptorIncrementSize); return rtvHandle; }
	CD3DX12_CPU_DESCRIPTOR_HANDLE getDsv() const { CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart()); return dsvHandle; }

	unsigned getCurrentFrame() const { return frameIndex; }
	unsigned getLastCompletedFrame() const { return lastCompletedFrame; }
private:
	static const UINT FrameCount = 2; //N buffers
	
	HWND hWnd = NULL;

	void enableDebugLayer();

	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adapter;
	bool createFactory();

	ComPtr<ID3D12Device5> device;
	bool createDevice();

	bool setupInfoQueue();

	ComPtr<ID3D12CommandQueue> commandQueue;
	bool createCommandQueue();

	ComPtr<IDXGISwapChain4> swapChain;
	bool createSwapChain();

	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	bool createRenderTargets();

	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Resource> depthStencil;
	bool createDepthStencil();
	
	ComPtr<ID3D12CommandAllocator> commandAllocators[FrameCount];
	ComPtr<ID3D12GraphicsCommandList> commandList;
	bool createCommandList();

	ComPtr<ID3D12Fence> Fence;
	bool createDrawFence();

	HANDLE fenceEvent;
	UINT windowWidth = 0;
	UINT windowHeight = 0;
	UINT rtvDescriptorIncrementSize;
	UINT currentBackBufferIndex;
	UINT64 fenceValues[FrameCount];
	UINT64 currentFenceValue;

	unsigned frameValues[FrameCount] = { 0, 0 };
	unsigned frameIndex = 0;
	unsigned lastCompletedFrame = 0;
	//UINT64 fenceCounter;
};