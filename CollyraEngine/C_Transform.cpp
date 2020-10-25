#include "C_Transform.h"
#include "Globals.h"

C_Transform::C_Transform() : Component(COMPONENT_TYPE::TRANSFORM, true), transform(float4x4::identity), position(0,0,0), rotation(Quat::identity), scale(1,1,1)
{
	GenerateEulerFromRot();
}

C_Transform::C_Transform(float3 pos, Quat rotation, float3 scale) : Component(COMPONENT_TYPE::TRANSFORM, true),
position(pos), rotation(rotation), scale(scale)
{
	SetTransformation(position, rotation, scale);
}

C_Transform::C_Transform(float4x4 transform) : Component(COMPONENT_TYPE::TRANSFORM, true), transform(transform)
{
	SetTransformation(transform);
}

C_Transform::~C_Transform()
{}

void C_Transform::Update(float dt)
{}

void C_Transform::SetTransformation(float4x4 transform)
{
	transform.Decompose(position, rotation, scale);
	GenerateEulerFromRot();
}

void C_Transform::SetTransformation(float3 pos, Quat rotation, float3 scale)
{
	this->position = pos;
	this->rotation = rotation;
	this->scale = scale;
	transform = float4x4::FromTRS(position, this->rotation, this->scale);
	GenerateEulerFromRot();
}

float4x4 C_Transform::GetTransform() const
{
	return transform;
}

float3 C_Transform::GetPosition() const
{
	return position;
}

Quat C_Transform::GetRotation() const
{
	return rotation;
}

float3 C_Transform::GetRotationEuler() const
{
	return rotationEuler;
}

float3 C_Transform::GetScale() const
{
	return scale;
}

void C_Transform::GenerateEulerFromRot()
{
	rotationEuler = rotation.ToEulerXYZ();
	rotationEuler *= RADTODEG;
}
