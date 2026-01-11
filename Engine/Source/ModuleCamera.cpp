#include "Globals.h"
#include "ModuleCamera.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "GamePad.h"
#include "Application.h"
#include "ModuleD3D12.h"

#include <algorithm>

#define MOVE_SPEED 1.5f
#define SENSITIVITY 2.0f / 1000
#define ZOOM_SENS 1.0f
#define FRAME_DIST_1 10.f
#define FRAME_DIST_2 5.f

ModuleCamera::ModuleCamera(ModuleD3D12* d3D12) : d3d12(d3D12)
{
}

bool ModuleCamera::init()
{
	app->getD3D12()->getWindowSize(windowWidth, windowHeight);
	//GamePad gamepad;

	eye = Vector3(0.0f, 5.0f, 10.0f);
	target = Vector3::Zero;
	up = Vector3::Up; 
	view = Matrix::CreateLookAt(eye, target, up);

	fovh = XM_PIDIV4; // PI/4
	aspect = float(windowWidth) / (windowHeight);
	nearZ = 0.1f; farZ = 100.0f;
	proj = Matrix::CreatePerspectiveFieldOfView(fovh, aspect, nearZ, farZ);

	forward = target - eye;
	forward.Normalize();

	yaw = atan2(forward.x, forward.z);
	pitch = asin(std::clamp(forward.y, -1.0f, 1.0f)); //funcion inversa para q mire inicialmente a 0,0

	frameFar = true; //sera para ir variando los tipos de framing, va alternando entre cerca y lejos
	fPressed = false;

	return true;
}

void ModuleCamera::update()
{
	Mouse& mouse = Mouse::Get();
	const Mouse::State& mouseState = mouse.GetState();

	Keyboard& keyboard = Keyboard::Get();
	const Keyboard::State& keyboardState = keyboard.GetState();

	float elapsedSec = app->getElapsedMilis() * 0.001f;

	Vector3 translate = Vector3::Zero;
	//Vector2 rotate = Vector2::Zero;

	float deltaWheel = lastWheel - mouseState.scrollWheelValue;

	//2 formas: quaternions o lookat. lo voy a hacer con el lookat de momento
	float fx = cos(pitch) * sin(yaw);
	float fy = sin(pitch);
	float fz = cos(pitch) * cos(yaw);

	forward = Vector3(fx, fy, fz);
	forward.Normalize();

	right = forward.Cross(up);
	right.Normalize();

	camUp = right.Cross(forward);
	camUp.Normalize();

	if (mouseState.rightButton /* && !keyboardState.LeftAlt*/)
	{
		//rotar camara
		yaw += float(lastX - mouseState.x) * SENSITIVITY;
		pitch += float(lastY - mouseState.y) * SENSITIVITY;
		pitch = std::clamp(pitch, -1.5f, 1.5f);

		//wasdqe
		Vector3 vecDist = target - eye;
		float dist = vecDist.Length(); //hay q preservar esta distancia, sino estaremos con el target pegado a la camara
		float moveSpeed = MOVE_SPEED;
		if (keyboardState.LeftShift) moveSpeed *= 2;
		if (keyboardState.W) eye += forward * moveSpeed * elapsedSec;
		if (keyboardState.A) eye -= right * moveSpeed * elapsedSec;
		if (keyboardState.S) eye -= forward * moveSpeed * elapsedSec;
		if (keyboardState.D) eye += right * moveSpeed * elapsedSec;
		if (keyboardState.Q) eye -= camUp * moveSpeed * elapsedSec;
		if (keyboardState.E) eye += camUp * moveSpeed * elapsedSec;

		target = eye + forward * dist;
	}

	if (mouseState.leftButton)
	{
		if (keyboardState.LeftAlt)
		{
			yaw += float(lastX - mouseState.x) * SENSITIVITY;
			pitch += float(lastY - mouseState.y) * SENSITIVITY;

			Vector3 vecDist = target - eye;
			float dist = vecDist.Length();

			eye = target - forward * dist;
		}
		/*else //El panning no hace falta en el assignment, descomentar para añadirlo
		{
			eye += right * float(mouseState.x - lastX) * SENSITIVITY;
			target += right * float(mouseState.x - lastX) * SENSITIVITY;
			eye += camUp * float(lastY - mouseState.y) * SENSITIVITY;
			target += camUp * float(lastY - mouseState.y) * SENSITIVITY;
		}*/
	}
	
	//if (keyboardState.LeftAlt)//CAMBIAR A MOUSE WHEEL para assignment, en nuestro caso añadir, prefiero tener ambas opciones
	//{
	//	if (mouseState.rightButton)
	//	{
	//		Vector3 vecDist = target - eye;
	//		float dist = vecDist.Length();
	//		forward = vecDist / dist;
	//		float zoomDelta = float(lastY - mouseState.y) * ZOOM_SENS * elapsedSec;
	//		if (zoomDelta != 0) {
	//			float newDist = dist - zoomDelta;
	//			newDist = std::clamp(newDist, 1.0f, 100.f);
	//			eye = target - forward * newDist;
	//		}
	//	}
	//}
	if(deltaWheel != 0)
	{
		Vector3 vecDist = target - eye;
		float dist = vecDist.Length();
		forward = vecDist / dist;
		float zoomDelta = deltaWheel * ZOOM_SENS * elapsedSec;
		if (zoomDelta != 0) {
			float newDist = dist + zoomDelta;
			newDist = std::clamp(newDist, 1.0f, 100.f);
			eye = target - forward * newDist;
		}
	}

	if (keyboardState.F)
	{
		if (!fPressed) {
			fPressed = true;
			target = Vector3::Zero; //de momento el target sera 0.0, mas adelante lo cambiaremos
			if (frameFar)
			{
				eye = target - forward * FRAME_DIST_1;
				frameFar = false;
			}
			else
			{
				eye = target - forward * FRAME_DIST_2;
				frameFar = true;
			}
		}
	}

	else fPressed = false;

	view = Matrix::CreateLookAt(eye, target, camUp);

	lastX = mouseState.x;
	lastY = mouseState.y;
	lastWheel = mouseState.scrollWheelValue;
}

void ModuleCamera::setFOV(const float newFOV)
{
	fovh = newFOV;
	proj = Matrix::CreatePerspectiveFieldOfView(fovh, aspect, nearZ, farZ);
}

void ModuleCamera::setNear(const float newNear)
{
	nearZ = newNear;
	proj = Matrix::CreatePerspectiveFieldOfView(fovh, aspect, nearZ, farZ);
}

void ModuleCamera::setFar(const float newFar)
{
	farZ = newFar;
	proj = Matrix::CreatePerspectiveFieldOfView(fovh, aspect, nearZ, farZ);
}

void ModuleCamera::setAspectRatio(const float width, const float height)
{
	aspect = width / height;
	proj = Matrix::CreatePerspectiveFieldOfView(fovh, aspect, 0.1f, 1000.0f);
}


/*
void ModuleCamera::setPlaneDistance()
{
}

void ModuleCamera::setPosition()
{
}

void ModuleCamera::setOrientation()
{
}
*/