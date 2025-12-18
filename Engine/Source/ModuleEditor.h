#pragma once

#include "Module.h"

class ImGuiPass;

class ModuleEditor : public Module
{
public:
	ModuleEditor(HWND wnd, ModuleD3D12* d3D12);
	//bool init();como este modulo depende de otros (moduled3d12), llamaremos a postinit()
	bool postInit();
	void preRender();
	void render();
	void update();
	//void postRender();
	bool cleanUp();

	void addLog(const char* msg);
	void addFramerate();
private:
	HWND hWnd = NULL;
	ModuleD3D12* d3d12 = nullptr;
	ImGuiPass* imGui = nullptr;
	std::vector<float> fps_log;
	std::vector<float> ms_log;

	float fov;
	float nearZ;
	float farZ;
	int samplerType;
	bool gridOn = true;
	bool axisOn = true;

	ID3D12Device2* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator; //?
	ID3D12CommandQueue* commandQueue;

	std::vector<std::string> logBuffer;
};

