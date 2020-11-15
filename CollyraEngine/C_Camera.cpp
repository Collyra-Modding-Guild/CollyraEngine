#include "C_Camera.h"
#include "GameObject.h"

C_Camera::C_Camera(bool active) : Component(COMPONENT_TYPE::MESH, active)
{ 
	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3(0, 0, 0);
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 1000.0f;


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
