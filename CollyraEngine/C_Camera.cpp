#include "C_Camera.h"
#include "GameObject.h"

C_Camera::C_Camera(bool active) : Component(COMPONENT_TYPE::CAMERA, active), verticalFOV(0.0f), horizontalFOV(90.0f), nearPlane(NEAR_PLANE),
farPlane(FAR_PLANE), culling(true)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);


	frustum.SetViewPlaneDistances(NEAR_PLANE, FAR_PLANE);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * horizontalFOV, 16.f / 9.f);

}

C_Camera::~C_Camera()
{ }

void C_Camera::UpdateFrustum(const float4x4& globalPosition)
{
	float3 position = float3(0, 0, 0);
	float3 scale = float3(1, 1, 1);
	Quat quat = Quat::identity;

	globalPosition.Decompose(position, quat, scale);

	frustum.SetPos(position);
	frustum.SetFront(globalPosition.WorldZ());
	frustum.SetUp(globalPosition.WorldY());
}

float C_Camera::GetHorizontalFov() const
{
	return frustum.HorizontalFov() * RADTODEG;
}

void C_Camera::SetHorizontalFov(float newHorizontalFov)
{
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * newHorizontalFov, frustum.AspectRatio());
}

float C_Camera::GetNearPlane() const
{
	return nearPlane;
}

void C_Camera::SetNearPlane(float newNearPlane)
{
	frustum.SetViewPlaneDistances(newNearPlane, frustum.FarPlaneDistance());
	nearPlane = newNearPlane;
}

float C_Camera::GetFarPlane() const
{
	return farPlane;
}

void C_Camera::SetFarPlane(float newFarPlane)
{
	frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), newFarPlane);
	farPlane = newFarPlane;
}

bool C_Camera::IsCulling() const
{
	return culling;
}

void C_Camera::SetCulling(bool newCulling)
{
	if (newCulling != culling)
	{
		culling = !culling;
	}
}


