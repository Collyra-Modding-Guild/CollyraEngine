#include "C_Transform.h"

C_Transform::C_Transform() : Component(COMPONENT_TYPE::TRANSFORM, true)
{}

C_Transform::~C_Transform()
{}

void C_Transform::Update(float dt)
{}

float4x4 C_Transform::GetTransform() const
{
	return transform;
}
