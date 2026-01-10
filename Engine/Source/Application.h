#pragma once

#include "Globals.h"

#include <array>
#include <vector>
#include <chrono>

class Module;
class ModuleD3D12; 
class ModuleEditor;
class ModuleResources;
class ModuleCamera;
class ModuleShaderDescriptors;
class ModuleSampler;
//class ModuleExercise4;
//class ModuleExercise5;
class ModuleExercise6;
class ModuleRingBuffer;

class Application
{
public:

	Application(int argc, wchar_t** argv, void* hWnd);
	~Application();

	bool         init();
	bool         postInit();
	void         update();
	bool         cleanUp();

    ModuleD3D12*                getD3D12() { return d3d12; }
    ModuleEditor*               getEditor() { return editor; }
    ModuleResources*            getResources() { return resources; }
    ModuleCamera*               getCamera() { return camera; }
	ModuleShaderDescriptors*    getShaderDescriptors() { return shaderDescriptors; }
	ModuleSampler*              getSampler() { return sampler; }
	ModuleRingBuffer*           getRingBuffer() { return ringBuffer; }
    
    //ModuleExercise4*            getCurrentExercise() { return exercise4; }
    //ModuleExercise5*            getCurrentExercise() { return exercise5; }
    ModuleExercise6*            getCurrentExercise() { return exercise6; }

    float                       getFPS() const { return 1000.0f * float(MAX_FPS_TICKS) / tickSum; }
    float                       getAvgElapsedMs() const { return tickSum / float(MAX_FPS_TICKS); }
    uint64_t                    getElapsedMilis() const { return elapsedMilis; }

    bool                        isPaused() const { return paused; }
    bool                        setPaused(bool p) { paused = p; return paused; }

    void addLog(const char* msg);

private:
    enum { MAX_FPS_TICKS = 30 };
    typedef std::array<uint64_t, MAX_FPS_TICKS> TickList;

    std::vector<Module*> modules;

    ModuleD3D12* d3d12 = nullptr;
    ModuleEditor* editor = nullptr;
    ModuleResources* resources = nullptr;
    ModuleCamera* camera = nullptr;
	ModuleShaderDescriptors* shaderDescriptors = nullptr;
	ModuleSampler* sampler = nullptr;
	//ModuleExercise4* exercise4 = nullptr;
	//ModuleExercise5* exercise5 = nullptr;
	ModuleExercise6* exercise6 = nullptr;
	ModuleRingBuffer* ringBuffer = nullptr;

    uint64_t  lastMilis = 0;
    TickList  tickList;
    uint64_t  tickIndex;
    uint64_t  tickSum = 0;
    uint64_t  elapsedMilis = 0;
    bool      paused = false;
};

extern Application* app;
