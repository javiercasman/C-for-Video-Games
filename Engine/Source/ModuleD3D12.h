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
	void getWindowSize(UINT& width, UINT& height);

	ID3D12Device5* getDevice() { return device.Get(); }
	ID3D12GraphicsCommandList* getCommandList() { return commandList.Get(); }
	ID3D12CommandAllocator* getCommandAllocator() { return commandAllocators[frameIndex].Get(); }
	ID3D12CommandQueue* getCommandQueue() { return commandQueue.Get(); }
	ID3D12Resource* getCurrentBackBuffer() { return renderTargets[frameIndex].Get(); }
	CD3DX12_CPU_DESCRIPTOR_HANDLE getCurrentRtv() { CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorIncrementSize); return rtvHandle; }
	CD3DX12_CPU_DESCRIPTOR_HANDLE getDsv() { CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart()); return dsvHandle; }
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
	UINT frameIndex;
	UINT64 fenceValues[FrameCount];
	UINT64 currentFenceValue;
	//UINT64 fenceCounter;
};