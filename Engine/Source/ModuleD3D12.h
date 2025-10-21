#pragma once

#include "Module.h"
#include <dxgi1_6.h>

class ModuleD3D12 : public Module
{
public:
	ModuleD3D12();
	bool Init();
	bool CleanUp();
private:
	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adapter;
	ComPtr<ID3D12Device5> device;
};