This second assignment is a small complex custom 3D engine developed in C++ using DirectX12, created as part of the C++ for Video Games subject from the Advanced Programming for AAA Video Games.

The goal of the engine is to explore low-level graphics programming concepts, such as GPU resource management, rendering pipelines, basic editor tooling, among others.

GitHub repository: https://github.com/javiercasman/C-for-Video-Games

## How to use the engine:
### Camera Controls (Editor Camera)
**Right Mouse Button** +
    - W / S → Forward / Backward
    - A / D → Left / Right
    - Q / E → Up / Down
    - Shift → Duplicate movement speed
    - Left Alt + Left Mouse Button + Drag → Orbit
    - Mouse Wheel Scroll → Zoom
    - F → Frame object at origin (0,0,0)

------------------------------------------------------------------------

### Editor / UI (ImGui)
**Console window**
    - Displays engine logs in real time
    - Clear button to reset output
**Performance window**
    - FPS histogram
    - Frame time (miliseconds) histogram
**Camera parameters**
    - Field of view (FOV)
    - Near Z plane
    - Far Z plane
**Sampler settings**
    - Linear / Point
    - Wrap / Clamp
**Debug options**
    - Toggle grid
    - Toggle axis

------------------------------------------------------------------------

### Notes for teachers
Main functionality of the engine is at “ModuleExercise7” class. 
ImGui interface is managed at “ModuleEditor” class.
“ModuleD3D12” preRender waits that commandAllocator of the active frame is free, and postRender closes the commandList and execute it. Render function is working at ModuleExercise4 and ModuleEditor.

Unfortunately, this second assignment wasn't successful for me. As you can check, the .exe won't open. The program crashes, and I honestly don't know why. I've been trying a lot, but I ran out of time to make it work properly.
If you check the source code you can check that functionalities are implemented, and it even runs compiling and running on VS (check last push, it works compiling and running on VS).

I tried to change the architecture, try to develop some kind of "exercise selector", but it was too ambitious for the little time I had, since other big functionalities also took a lot of time from me.

In the developing process I have reached a point of no return, realizing that no matter what I would do, I couldn't deliver a program that could run properly. I am sad, but it is what it is, I have learned my lesson for next time.