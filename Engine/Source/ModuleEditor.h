#pragma once

#include "Module.h"
#include "ModuleD3D12.h"
#include "ImGuiPass.h"
#include "Application.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor(HWND wnd);
	//bool init();como este modulo depende de otros (moduled3d12), llamaremos a postinit()
	bool postInit();
	void preRender();
	void render();
	void update();
	//void postRender();
	bool cleanUp();
	void AddLog(const char* msg);
private:
	HWND hWnd = NULL;
	ModuleD3D12* d3d12 = nullptr;
	ImGuiPass* imGui = nullptr;

	ComPtr<ID3D12Device2> device;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12CommandQueue> commandQueue;

	std::vector<std::string> logBuffer;
};

