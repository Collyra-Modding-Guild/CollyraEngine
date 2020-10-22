#pragma once
#include "Module.h"
#include "glmath.h"

class M_Camera3D : public Module
{
public:
	M_Camera3D(MODULE_TYPE type, bool start_enabled = true);
	~M_Camera3D();

	bool Start();
	updateStatus Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	vec3 GetCameraPosition();

private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference, camDistanceFromCar, newCameraPos;
	mat3x3 rotation;

	bool editorCamera;
	float spdMultiplier;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};