#pragma once
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"

#define DEFAULT_MOUSE_SPEED 10.0f
#define DEFAULT_WHEEL_SPEED 100.0f

class GameObject;
class C_Camera;

class M_Camera3D : public Module
{
public:
	M_Camera3D(MODULE_TYPE type, bool startEnabled = true);
	~M_Camera3D();

	bool Start();
	updateStatus Update(float dt);
	bool CleanUp();

	void Look(const float3&Position, const float3&Reference, bool RotateAroundReference = false);
	void OrbitalLook(const float3&Position, const float3&Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);
	void Move(const float3& Movement, bool changeReference = true);
	void FocusGameObject(GameObject* focused, float multiplier = 1.0f);

	float* GetViewMatrix();
	float3 GetCameraPosition();

	void ShootRay(float2 mousePosition);


private:
	void CalculateViewMatrix();
	void CameraMovement(float dt);
	void CameraPlanePan(float3& newPos);
	void OrbitArroundReference(float3& reference, float3&pos, float speed);
	void ResetReference();
	float3 CameraZoom(float dt);

public:
	
	float3		Reference, orbitalReference;

	bool		focusingObject;
	float		spdMultiplier;

	LineSegment ray;

private:

	M_Input*	inputModule;
	float4		ViewMatrix, ViewMatrixInverse;
	C_Camera*	sceneCamera;

	

};