#pragma once

#include "Module.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	bool init();
	void update();

	void setFOV(float newFOV); //Set the horizontal FOV keeping the aspect ratio
	void setNear(float newNear); //Set the horizontal FOV keeping the aspect ratio
	void setFar(float newFar); //Set the horizontal FOV keeping the aspect ratio
	void setAspectRatio(); //Change the vertical FOV to meet the new aspect ratio
	void setPlaneDistance();
	void setPosition();
	void setOrientation();

	Matrix getViewMatrix() { return view; }
	Matrix getProjectionMatrix() { return proj; }
	float getFOV() { return fovh; }
	float getNear() { return nearZ; }
	float getFar() { return farZ; }

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

	int lastX, lastY;

	Vector3 eye;
	Vector3 target;
	Vector3 up;

	Vector3 forward;
	Vector3 right;
	Vector3 camUp;
};