#include "Globals.h"
#include "ModuleEditor.h"
#include "Module.h"
#include "ModuleD3D12.h"
#include "ModuleCamera.h"
#include "Application.h"
#include "ImGuiPass.h"
#include "ModuleSampler.h"
#include "ModuleExercise4.h"
#include "ModuleExercise5.h"
#include "ModuleExercise6.h"
#include "ImGuizmo.h"
#include "Model.h"


ModuleEditor::ModuleEditor(HWND wnd, ModuleD3D12* d3D12) : hWnd(wnd), d3d12(d3D12)
{
	gizmoOperation = ImGuizmo::TRANSLATE;
}

bool ModuleEditor::cleanUp()
{
	//imGui->~ImGuiPass();
	return true;
}

bool ModuleEditor::postInit()
{
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

	samplerType = int(ModuleSampler::LINEAR_WRAP);

	return true;
}

void ModuleEditor::preRender()
{
	imGui->startFrame();
	ImGuizmo::BeginFrame();
	UINT width, height;
	d3d12->getWindowSize(width, height);
	ImGuizmo::SetRect(0, 0, float(width), float(height));
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
	/*
	std::vector<std::string> str;
	int exerciseCount = app->getExerciseCount();

	for (int i = 2; i <= exerciseCount; ++i) {
		str.push_back("Exercise " + std::to_string(i));
	}

	std::vector<const char*> exerciseOptions;
	for (const auto& s : str) {
		exerciseOptions.push_back(s.c_str()); 
	}

	int exerciseIndex = -1;

	ImGui::Begin("Exercise selector");

	if (ImGui::Combo("Type", &exerciseIndex, exerciseOptions.data(), exerciseCount))
	{
		app->setCurrentExercise(exerciseIndex);
	}

	ImGui::End();
	*/

	/*
	switch (app->getCurrentExerciseIndex())
	{
		case 2:
			exercise2GUI();
			break;
		case 3:
			exercise3GUI();
			break;
		case 4:
			exercise4GUI();
			break;
		case 5:
			exercise5GUI();
			break;
		case 6:
			exercise6GUI();
			break;
		default:
			break;
	}
	*/
	exercise6GUI();

	imGui->record(commandList);
}

void ModuleEditor::update()
{
	/*if (fov != app->getCamera()->getFOV()) 
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
	if(ModuleSampler::Type(samplerType) != app->getCurrentExercise()->getSamplerType())
	{
		app->getCurrentExercise()->setSampler(ModuleSampler::Type(samplerType));
	}*/
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
	if (fps_log.size() > max_samples) fps_log.pop_front();
	if (ms_log.size() > max_samples) ms_log.pop_front();
}

void ModuleEditor::exercise2GUI()
{
	//nada
}

void ModuleEditor::exercise3GUI()
{
	//nada
}

void ModuleEditor::exercise4GUI()
{
	ImGui::Begin("Console");

	if (ImGui::Button("Clear")) logBuffer.clear();

	ImGui::Separator();

	for (const auto& line : logBuffer) ImGui::TextUnformatted(line.c_str());

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);

	ImGui::End();

	ImGui::Begin("Performance");
	addFramerate();
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", getHistogramValue, &fps_log, fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", getHistogramValue, &ms_log, ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

	ImGui::End();

	ImGui::Begin("Camera parameters");

	if (ImGui::DragFloat("FOV", &fov, 0.01f, 0.2f, 2.5f))
	{
		app->getCamera()->setFOV(fov);
	}

	if (ImGui::DragFloat("Near Z", &nearZ, 0.01f, 0.01f, farZ - 0.01f))
	{
		app->getCamera()->setNear(nearZ);
	}

	if (ImGui::DragFloat("Far Z", &farZ, 1.0f, nearZ + 0.01f, 100.0f))
	{
		app->getCamera()->setFar(farZ);
	}

	ImGui::End();

	ImGui::Begin("Sampler");

	static const char* samplerNames[] =
	{
		"Linear / Wrap",
		"Point / Wrap",
		"Linear / Clamp",
		"Point / Clamp"
	};

	if (ImGui::Combo("Type", &samplerType, samplerNames, ModuleSampler::COUNT))
	{
		app->getCurrentExercise()->setSampler(ModuleSampler::Type(samplerType));
	}

	ImGui::End();

	ImGui::Begin("Debug");

	if (ImGui::Checkbox("Show Axis", &axisOn))
	{
		app->getCurrentExercise()->setShowAxis(axisOn);
	}
	if (ImGui::Checkbox("Show Grid", &gridOn))
	{
		app->getCurrentExercise()->setShowGrid(gridOn);
	}
}

void ModuleEditor::exercise5GUI()
{
	exercise4GUI();
	if (ImGui::Checkbox("Show Guizmo", &guizmoOn))
	{
		app->getCurrentExercise()->setShowGrid(guizmoOn);
	}

	Matrix objectMatrix = app->getCurrentExercise()->getModel()->getModelMatrix();

	static ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_W)) gizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E)) gizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R)) gizmoOperation = ImGuizmo::SCALE;

	ImGui::RadioButton("Translate", (int*)&gizmoOperation, (int)ImGuizmo::TRANSLATE);
	ImGui::SameLine();
	ImGui::RadioButton("Rotate", (int*)&gizmoOperation, ImGuizmo::ROTATE);
	ImGui::SameLine();
	ImGui::RadioButton("Scale", (int*)&gizmoOperation, ImGuizmo::SCALE);

	float translation[3], rotation[3], scale[3];
	ImGuizmo::DecomposeMatrixToComponents((float*)&objectMatrix, translation, rotation, scale);
	bool transform_changed = ImGui::DragFloat3("Tr", translation, 0.1f);
	transform_changed = transform_changed || ImGui::DragFloat3("Rt", rotation, 0.1f);
	transform_changed = transform_changed || ImGui::DragFloat3("Sc", scale, 0.1f);

	if (transform_changed)
	{
		ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, (float*)&objectMatrix);

		app->getCurrentExercise()->getModel()->setModelMatrix(objectMatrix);
	}

	ImGui::End();

	if (guizmoOn)
	{
		const Matrix& viewMatrix = app->getCamera()->getViewMatrix();
		const Matrix& projMatrix = app->getCamera()->getProjectionMatrix();

		// Manipulate the object
		ImGuizmo::Manipulate((const float*)&viewMatrix, (const float*)&projMatrix, gizmoOperation, ImGuizmo::LOCAL, (float*)&objectMatrix);
	}

	if (ImGuizmo::IsUsing())
	{
		//no entra
		app->getCurrentExercise()->getModel()->setModelMatrix(objectMatrix);
	}
}

void ModuleEditor::exercise6GUI()
{
	exercise5GUI();

	ModuleExercise6::Light& light = app->getCurrentExercise()->getLight();
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Light Direction", reinterpret_cast<float*>(&light.L), 0.1f, -1.0f, 1.0f);
		ImGui::SameLine();
		if (ImGui::SmallButton("Normalize"))
		{
			light.L.Normalize();
		}
		ImGui::ColorEdit3("Light Colour", reinterpret_cast<float*>(&light.Lc), ImGuiColorEditFlags_NoAlpha);
		ImGui::ColorEdit3("Ambient Colour", reinterpret_cast<float*>(&light.Ac), ImGuiColorEditFlags_NoAlpha);
	}

	for (Material* material : app->getCurrentExercise()->getModel()->getMaterials())
	{
		if (material->getMaterialType() == Material::Phong)
		{
			char tmp[256];
			_snprintf_s(tmp, 255, "Material %s", material->getName());

			if (ImGui::CollapsingHeader(tmp, ImGuiTreeNodeFlags_DefaultOpen))
			{
				PhongMaterialData phong = material->getPhongMaterial();
				if (ImGui::ColorEdit3("Diffuse Colour", reinterpret_cast<float*>(&phong.diffuseColour)))
				{
					material->setPhongMaterial(phong);
				}

				bool hasTexture = phong.hasDiffuseTexture;
				if (ImGui::Checkbox("Use Texture", &hasTexture))
				{
					phong.hasDiffuseTexture = hasTexture;
					material->setPhongMaterial(phong);
				}

				if (ImGui::DragFloat("Kd", &phong.kd, 0.01f))
				{
					material->setPhongMaterial(phong);
				}

				if (ImGui::DragFloat("Ks", &phong.ks, 0.01f))
				{
					material->setPhongMaterial(phong);
				}

				if (ImGui::DragFloat("shininess", &phong.shininess))
				{
					material->setPhongMaterial(phong);
				}
			}
		}
	}
}