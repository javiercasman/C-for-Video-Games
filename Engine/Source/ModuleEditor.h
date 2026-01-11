#pragma once

#include "Module.h"
#include <deque>

class ImGuiPass;
class RenderTexture;
struct Light;

namespace ImGuizmo 
{
	enum OPERATION;
}


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

	void exercise7GUI();
private:
	HWND hWnd = NULL;
	ModuleD3D12* d3d12 = nullptr;
	ImGuiPass* imGui = nullptr;
	std::deque<float> fps_log;
	std::deque<float> ms_log;

	float fov;
	float nearZ;
	float farZ;
	int samplerType;
	bool gridOn = true;
	bool axisOn = true;
	bool guizmoOn = true;

	ID3D12Device2* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator; //?
	ID3D12CommandQueue* commandQueue;

	std::vector<std::string> logBuffer;

	static float getHistogramValue(void* data, int idx)
	{
		auto* deque = static_cast<std::deque<float>*>(data);
		return (*deque)[idx];
	}

	ImGuizmo::OPERATION gizmoOperation;

	ImVec2 canvasPos;
	RenderTexture* renderTexture;

	void exercise2GUI();
	void exercise3GUI();
	void exercise4GUI();
	void exercise5GUI();
	void exercise6GUI();

	void renderToTexture();
};

