#include "Globals.h"
#include "ModuleD3D12.h"
#include "Application.h"

ModuleD3D12::ModuleD3D12(HWND wnd) : hWnd(wnd), fenceValues{}, rtvDescriptorIncrementSize(0), frameIndex(0), fenceEvent(nullptr), currentFenceValue(0)
{
}

bool ModuleD3D12::init()
{
	//Enable debug layer
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugInterface;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	debugInterface->EnableDebugLayer();
#endif

	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	if(FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)))) return false;

	factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));

	if(FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)))) return false;

#if defined(_DEBUG)
	device.As(&infoQueue);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
#endif

	//command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))) return false;

	GetWindowSize(windowWidth, windowHeight);

	//swap chain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = windowWidth; // Width of the back buffer in pixels
	swapChainDesc.Height = windowHeight; // Height of the back buffer in pixels
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit RGBA format (8 bits per channel)
	// UNORM = Unsigned normalized integer (0-255 mapped to 0.0-1.0)
	swapChainDesc.Stereo = FALSE; // Set to TRUE for stereoscopic 3D rendering (VR/3D Vision)
	swapChainDesc.SampleDesc = { 1, 0 }; // Multisampling { Count, Quality } // Count=1: No multisampling (1 sample per pixel)
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // This buffer will be used as a render target
	swapChainDesc.BufferCount = FrameCount; // Double buffering:
	// - 1 front buffer (displayed)
   // - 1 back buffer (rendering)
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // How to scale when window size doesn't match buffer size:
	// STRETCH = Stretch the image to fit the window
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Modern efficient swap method:
	// - FLIP: Uses page flipping (no copying)
   // - DISCARD: Discard previous back buffer contents
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // Alpha channel behavior for window blending UNSPECIFIED = Use default behavior
	swapChainDesc.Flags = 0; // Additional swap chain options: 0 = No special flags
	// DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH: Allow full-screen mode switches
   // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING: Allow tearing in windowed mode (VSync off)
 
	ComPtr<IDXGISwapChain1> swapChain1;
	if (FAILED(factory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1))) return false;

	if (FAILED(swapChain1.As(&swapChain))) return false; //QueryInterface
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) return false;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	rtvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

	for (UINT i = 0; i < FrameCount; ++i) {
		if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])))) return false;
		device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvDescriptorIncrementSize); //incrementar by 1 descriptor la size rtvDescr...
		if (FAILED(device->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&commandAllocators[i])))) return false;
	}

	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList)))) return false;
	if (FAILED(commandList->Close())) return false;

	if (FAILED(device->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)))) return false;
	fenceValues[frameIndex]++;

	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		if (FAILED(HRESULT_FROM_WIN32(GetLastError()))) return false;
	}
	// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.
	//WaitForGPU();

    return true; //cambiar, podemos poner true como predeterminado pero mejor usar SUCCEEDED(hr) para ver si ha salido bien o no
}

bool ModuleD3D12::CleanUp()
{
	WaitForGPU();
	if (fenceEvent) CloseHandle(fenceEvent);
	fenceEvent = NULL;
	return true;
}

void ModuleD3D12::update()
{

}

void ModuleD3D12::preRender()
{
	//Wait for fence, Reset allocator for current back buffer.
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	//Esperar a que el CommandAllocator del frame actual esté libre (usando la fence).
	if (Fence->GetCompletedValue() < fenceValues[frameIndex])
	{
		Fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	//Llamar a Reset() en el CommandAllocator y en la CommandList.
	commandAllocators[frameIndex]->Reset();
	commandList->Reset(commandAllocators[frameIndex].Get(), nullptr);
}

void ModuleD3D12::postRender()
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);
	commandList->Close();

	//no sabemos cuantas command lists hay, asi que:
	ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(std::size(ppCommandLists), ppCommandLists);

	swapChain->Present(1, 0);
	
	currentFenceValue = fenceValues[frameIndex] + 1;
	//SIGNAL
	commandQueue->Signal(Fence.Get(), currentFenceValue);
	fenceValues[frameIndex] = currentFenceValue;

	frameIndex = swapChain->GetCurrentBackBufferIndex();
}

void ModuleD3D12::render()
{
	//Transicionar el back buffer desde PRESENT --> RENDER_TARGET con un ResourceBarrier.
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrier);

	//Configurar el Render Target View (RTV) para ese back buffer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorIncrementSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	//const float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float colorRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, colorRed, 0, nullptr);
}

void ModuleD3D12::GetWindowSize(UINT &width, UINT &height) 
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void ModuleD3D12::WaitForGPU()
{
	commandQueue->Signal(Fence.Get(), ++currentFenceValue);
	Fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
	WaitForSingleObject(fenceEvent, INFINITE);
}

bool ModuleD3D12::resize()
{
	unsigned width, height;
	GetWindowSize(width, height);

	if (width != windowWidth || height != windowHeight)
	{
		windowWidth = width;
		windowHeight = height;

		WaitForGPU();

		for (unsigned i = 0; i < FrameCount; ++i)
		{
			renderTargets[i].Reset();
			fenceValues[i] = 0;
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

		if (FAILED(swapChain->GetDesc(&swapChainDesc))) return false;
		if (FAILED(swapChain->ResizeBuffers(FrameCount, windowWidth, windowHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags))) return false;

		if (windowWidth > 0 && windowHeight > 0)
		{
			//ok = ok && createRenderTargets();
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.NumDescriptors = FrameCount;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) return false;

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
			rtvDescriptorIncrementSize = device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);

			for (UINT i = 0; i < FrameCount; ++i) {
				if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])))) return false;
				device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, rtvDescriptorIncrementSize); //incrementar by 1 descriptor la size rtvDescr...
			}
			//ok = ok && createDepthStencil();
		}
	}
}

