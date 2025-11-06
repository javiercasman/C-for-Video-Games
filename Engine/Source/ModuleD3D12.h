#pragma once

#include "Module.h"
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
	void WaitForGPU();
	bool CleanUp();
	void GetWindowSize(UINT& width, UINT& height);
private:
	static const UINT FrameCount = 2; //N buffers
	
	HWND hWnd = NULL;

	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device5> device;
	ComPtr<ID3D12InfoQueue> infoQueue;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocators[FrameCount];
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Fence> Fence;

	HANDLE fenceEvent;
	UINT windowWidth = 0;
	UINT windowHeight = 0;
	UINT rtvDescriptorIncrementSize;
	UINT frameIndex;
	UINT64 fenceValues[FrameCount];
	UINT64 currentFenceValue;
	//UINT64 fenceCounter;
};