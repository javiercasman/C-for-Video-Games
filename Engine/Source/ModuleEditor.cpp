#include "Globals.h"
#include "ModuleEditor.h"
#include "Module.h"
#include "ModuleD3D12.h"
#include "ModuleCamera.h"
#include "Application.h"


ModuleEditor::ModuleEditor(HWND wnd) : hWnd(wnd)
{
}

bool ModuleEditor::cleanUp()
{
	//imGui->~ImGuiPass();
	return true;
}

bool ModuleEditor::postInit()
{
	d3d12 = app->getD3D12();
	if (FAILED(d3d12->getDevice()->QueryInterface(&device))) return false;
	////if (FAILED(swapChain1.As(&swapChain))) return false;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuTextHandle = { 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE gpuTextHandle = { 0 };

	imGui = new ImGuiPass(device, hWnd, cpuTextHandle, gpuTextHandle);
	commandList = d3d12->getCommandList();
	//commandQueue = d3d12->getCommandQueue();

	fps_log.push_back(0.0f);
	ms_log.push_back(0.0f);

	fov = app->getCamera()->getFOV();
	nearZ = app->getCamera()->getNear(); farZ = app->getCamera()->getFar();

	return true;
}

void ModuleEditor::preRender()
{
	imGui->startFrame();
}

void ModuleEditor::render()
{
	//ImGui::ShowDemoWindow();

	//if (ImGui::BeginMenu("Help"))
	//{
	//	if (ImGui::MenuItem("Gui Demo"));
	//	//showcase = !showcase;
	//	if (ImGui::MenuItem("Documentation"));
	//	//app->RequestBrowser("https://github.com/d0n3val/Edu-Game-Engine/wiki");
	//	if (ImGui::MenuItem("Download latest"));
	//	//app->RequestBrowser("https://github.com/d0n3val/Edu-Game-Engine/releases");
	//	if (ImGui::MenuItem("Report a bug"));
	//	//app->RequestBrowser("https://github.com/d0n3val/Edu-Game-Engine/issues");
	//	if (ImGui::MenuItem("About"));
	//	//about->SwitchActive();
	//	ImGui::EndMenu();
	//}
	//ImGui::EndMainMenuBar();

	ImGui::Begin("Console");

	if (ImGui::Button("Clear")) logBuffer.clear();

	ImGui::Separator();

	for (const auto& line : logBuffer) ImGui::TextUnformatted(line.c_str());

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);

	addFramerate();
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	ImGui::End();

	ImGui::Begin("Params");

	ImGui::SliderFloat("FOV", &fov, 0.7f, 1.2f);
	ImGui::SliderFloat("Near Z", &nearZ, 0.1f, 1000.f);
	ImGui::SliderFloat("Far Z", &farZ, 0.1f, 1000.f); //nearz no puede ser igual a farz (crashea)

	ImGui::End();

	imGui->record(commandList);
}

void ModuleEditor::update()
{
	if (fov != app->getCamera()->getFOV()) 
	{
		app->getCamera()->setFOV(fov);
	}
	if (nearZ != app->getCamera()->getNear()) 
	{
		app->getCamera()->setNear(nearZ);
	}
	if (farZ != app->getCamera()->getFar()) 
	{
		app->getCamera()->setFar(farZ);
	}

}

void ModuleEditor::addLog(const char* msg)
{
	if (msg && msg[0] != '\0')
		logBuffer.emplace_back(msg);
}

void ModuleEditor::addFramerate()
{
	float ms = app->getAvgElapsedMs();
	float fps = app->getFPS();

	fps_log.push_back(fps);
	ms_log.push_back(ms);

	// Limitar tamaño (ejemplo: 200 muestras)
	const int max_samples = 100;
	if (fps_log.size() > max_samples) fps_log.erase(fps_log.begin());
	if (ms_log.size() > max_samples) ms_log.erase(ms_log.begin());
}