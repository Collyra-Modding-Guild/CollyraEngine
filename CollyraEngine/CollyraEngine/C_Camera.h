#pragma once

#include "Component.h"
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"

struct GameObject;

#define NEAR_PLANE 0.1f
#define FAR_PLANE 150.f

class C_Camera : public Component
{
public:

	C_Camera(bool active = true);
	~C_Camera();

	void UpdateFrustum(const float4x4& globalPosition);

	float GetHorizontalFov() const;
	void SetHorizontalFov(float newHorizontalFov);

	float GetNearPlane() const;
	void SetNearPlane(float newNearPlane);

	float GetFarPlane() const;
	void SetFarPlane(float newFarPlane);

	bool IsCulling() const;
	void SetCulling(bool newCulling);

	void SetAspectRatio(float aspectRatio);
	float4x4 GetProjectionMatrix();
	float4x4 GetViewMatrix();
	float3 GetCamPosition();


public:

	Frustum frustum;

private:

	float nearPlane, farPlane;
	float verticalFOV, horizontalFOV;
	bool culling;


};