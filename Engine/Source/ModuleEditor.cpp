#include "Globals.h"
#include "ModuleEditor.h"

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
	if (FAILED(d3d12->getDevice().As(&device))) return false;
	////if (FAILED(swapChain1.As(&swapChain))) return false;
	D3D12_CPU_DESCRIPTOR_HANDLE cpuTextHandle = { 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE gpuTextHandle = { 0 };

	imGui = new ImGuiPass(device.Get(), hWnd, cpuTextHandle, gpuTextHandle);
	commandList = d3d12->getCommandList();
	//commandQueue = d3d12->getCommandQueue();
	return true;
}

void ModuleEditor::preRender()
{
	imGui->startFrame();
}

void ModuleEditor::render()
{
	ImGui::ShowDemoWindow();

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Gui Demo"));
		//showcase = !showcase;
		if (ImGui::MenuItem("Documentation"));
		//app->RequestBrowser("https://github.com/d0n3val/Edu-Game-Engine/wiki");
		if (ImGui::MenuItem("Download latest"));
		//app->RequestBrowser("https://github.com/d0n3val/Edu-Game-Engine/releases");
		if (ImGui::MenuItem("Report a bug"));
		//app->RequestBrowser("https://github.com/d0n3val/Edu-Game-Engine/issues");
		if (ImGui::MenuItem("About"));
		//about->SwitchActive();
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	ImGui::Begin("Console");

	if (ImGui::Button("Clear"))
		logBuffer.clear();

	ImGui::Separator();

	for (const auto& line : logBuffer)
		ImGui::TextUnformatted(line.c_str());

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::End();

	imGui->record(commandList.Get());
}

void ModuleEditor::update()
{
	
}

void ModuleEditor::AddLog(const char* msg)
{
	if (msg && msg[0] != '\0')
		logBuffer.emplace_back(msg);
}