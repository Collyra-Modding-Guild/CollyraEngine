#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#include "SDL/include/SDL_rect.h"

#define LERP_VALUE 0.25


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	vec3 GetCameraPosition();
	SDL_Rect getViewPort();


private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference, camDistanceFromCar, newpos;
	mat3x3 rotation;

	bool cameraDebug = false;
	bool lookAtBall = false;
	float multiplier = 1.0f;

	int cameraNum = 0;
	SDL_Rect camViewPort;


private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};