#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleD3D12.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"
#include "ModuleExercise2.h"
#include "ModuleExercise3.h"
#include "ModuleExercise4.h"
#include "ModuleExercise5.h"
#include "ModuleCamera.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleSampler.h"


Application::Application(int argc, wchar_t** argv, void* hWnd)
{
    //aqui añadimos los modulos a modules. tendremos que hacerlo con ModuleD3D12
    modules.push_back(new ModuleInput((HWND)hWnd));
    modules.push_back(d3d12 = new ModuleD3D12((HWND)hWnd));
    modules.push_back(camera = new ModuleCamera(d3d12));
    modules.push_back(resources = new ModuleResources(d3d12));
    modules.push_back(shaderDescriptors = new ModuleShaderDescriptors(d3d12));
    modules.push_back(sampler = new ModuleSampler(d3d12));
    //modules.push_back(new ModuleExercise2(d3d12));
    //modules.push_back(new ModuleExercise3(d3d12, camera));
	//modules.push_back(exercise4 = new ModuleExercise4(d3d12, camera));
	modules.push_back(exercise5 = new ModuleExercise5(d3d12, camera));
    modules.push_back(editor = new ModuleEditor((HWND)hWnd, d3d12)); //ModuleEditor no funciona por si solo. tiene que ir despues de un modulo q renderize (los ejercicios)
}

Application::~Application()
{
    cleanUp();

	for(auto it = modules.rbegin(); it != modules.rend(); ++it)
    {
        delete *it;
    }
}
 
bool Application::init()
{
	bool ret = true;

	for(auto it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->init();

    lastMilis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	return ret;
}

bool Application::postInit()
{
    bool ret = true;

   for (auto it = modules.begin(); it != modules.end() && ret; ++it)
        ret = (*it)->postInit();

    lastMilis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    return ret;
}

void Application::update()
{
    using namespace std::chrono_literals;

    // Update milis
    uint64_t currentMilis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    elapsedMilis = currentMilis - lastMilis;
    lastMilis = currentMilis;
    tickSum -= tickList[tickIndex];
    tickSum += elapsedMilis;
    tickList[tickIndex] = elapsedMilis;
    tickIndex = (tickIndex + 1) % MAX_FPS_TICKS;

    if (!app->paused)
    {
        for (auto it = modules.begin(); it != modules.end(); ++it)
            (*it)->update();

        for (auto it = modules.begin(); it != modules.end(); ++it)
            (*it)->preRender();

        for (auto it = modules.begin(); it != modules.end(); ++it)
            (*it)->render();

        for (auto it = modules.begin(); it != modules.end(); ++it)
            (*it)->postRender();
    }
}

bool Application::cleanUp()
{
	bool ret = true;

	for(auto it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->cleanUp();

	return ret;
}

void Application::addLog(const char* msg) 
{ 
    if (editor) editor->addLog(msg); 
}