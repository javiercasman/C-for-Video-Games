#pragma once

#include "Module.h"
#include <dxgi1_6.h>

class ModuleD3D12 : public Module
{
public:
	ModuleD3D12(HWND wnd);
	bool Init();
	bool CleanUp();
	void preRender();
	void GetWindowSize(UINT& width, UINT& height);
private:
	static const UINT FrameCount = 2; //N buffers para swapchain y rtv
	
	HWND hWnd = NULL;
	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device5> device;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<IDXGISwapChain1> swapChain;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTargets[FrameCount];
	UINT windowWidth = 0;
	UINT windowHeight = 0;
};