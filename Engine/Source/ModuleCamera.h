#pragma once

#include "Module.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera(ModuleD3D12* d3D12);

	bool init();
	void update();

	void setFOV(const float newFOV); //Set the horizontal FOV keeping the aspect ratio
	void setNear(const float newNear); //Set the horizontal FOV keeping the aspect ratio
	void setFar(const float newFar); //Set the horizontal FOV keeping the aspect ratio
	void setAspectRatio(); //Change the vertical FOV to meet the new aspect ratio
	//void setPlaneDistance();
	//void setPosition();
	//void setOrientation();

	Matrix getViewMatrix() const { return view; }
	Matrix getProjectionMatrix() const { return proj; }
	float getFOV() const { return fovh; }
	float getNear() const { return nearZ; }
	float getFar() const { return farZ; }

private:
	ModuleD3D12* d3d12;

	HWND hWnd = NULL;

	Matrix proj;
	Matrix view;

	unsigned windowWidth, windowHeight;
	float fovh;
	float aspect;
	float nearZ;
	float farZ;
	float yaw, pitch;

	bool frameFar;
	bool fPressed;

	int lastX, lastY, lastWheel;

	Vector3 eye;
	Vector3 target;
	Vector3 up;

	Vector3 forward;
	Vector3 right;
	Vector3 camUp;
};