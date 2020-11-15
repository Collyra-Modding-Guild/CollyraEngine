#include "C_Camera.h"
#include "GameObject.h"

C_Camera::C_Camera(bool active) : Component(COMPONENT_TYPE::MESH, active), verticalFOV(0.0f), horizontalFOV(90.0f)
{ 

	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0, 0, 0);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 150.0f;

	frustum.horizontalFov = DEGTORAD * horizontalFOV;
	HOR(16, 9);

}

C_Camera::~C_Camera()
{ }

void C_Camera::UpdateFrustum(const float4x4& globalPosition)
{
	float3 position = float3(0, 0, 0);
	float3 scale = float3(1, 1, 1);
	Quat quat = Quat::identity;

	globalPosition.Decompose(position, quat, scale);

	frustum.pos = position;
}

void C_Camera::HOR(uint aspectWidth, uint aspectHeight)
{
	frustum.verticalFov = 2 * atanf(tanf(frustum.horizontalFov / 2) * ((float)aspectHeight / (float)aspectWidth));
}
