#include "Globals.h"
#include "ModuleD3D12.h"

bool ModuleD3D12::Init()
{
	//Enable debug layer
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugInterface;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	debugInterface->EnableDebugLayer();
#endif

	//ComPtr<IDXGIFactory6> factory; //mejor declararlo como miembro, mas adelante querremos usarlo

#if defined(_DEBUG)
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
	CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
#endif

	//ComPtr<IDXGIAdapter4> adapter;
	//ComPtr<ID3D12Device5> device; //declarar tambien como miembros
	factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

	ComPtr<ID3D12InfoQueue> infoQueue;
	device.As(&infoQueue);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

    return true; //cambiar, podemos poner true como predeterminado pero mejor usar SUCCEEDED(hr) para ver si ha salido bien o no
}

bool ModuleD3D12::CleanUp()
{
	return true;
}
