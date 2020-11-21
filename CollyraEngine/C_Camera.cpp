#include "C_Camera.h"
#include "GameObject.h"

C_Camera::C_Camera(bool active) : Component(COMPONENT_TYPE::CAMERA, active), verticalFOV(0.0f), horizontalFOV(90.0f)
{
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);

	frustum.SetViewPlaneDistances(0.1f, 512.0f);
	frustum.SetHorizontalFovAndAspectRatio(DEGTORAD * horizontalFOV, 1231 / 819);

	//HOR(16, 9);
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
	return horizontalFOV;
}

void C_Camera::SetHorizontalFov(float horizontalFov)
{
	horizontalFOV = horizontalFov;
	HOR(16, 9);
}

float C_Camera::GetNearPlane() const
{
	return 1.0f;
}

void C_Camera::SetNearPlane(float nearPlane)
{
	//frustum.nearPlaneDistance = nearPlane;
}

float C_Camera::GetFarPlane() const
{
	return 1.0f;
}

void C_Camera::SetFarPlane(float farPlane)
{
	//frustum.Set = farPlane;
}

void C_Camera::HOR(uint aspectWidth, uint aspectHeight)
{
	//frustum.verticalFov = 2 * atanf(tanf(frustum.horizontalFov / 2) * ((float)aspectHeight / (float)aspectWidth));
}
