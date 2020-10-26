#include "C_Transform.h"
#include "Globals.h"

C_Transform::C_Transform() : Component(COMPONENT_TYPE::TRANSFORM, true), localTransform(float4x4::identity), globalTransform(float4x4::identity),
position(0, 0, 0), rotation(Quat::identity), scale(1, 1, 1), tGlobalTransform(float4x4::identity)
{
	GenerateEulerFromRot();
}

C_Transform::C_Transform(float4x4 parentTransform, float3 pos, Quat rotation, float3 scale) : Component(COMPONENT_TYPE::TRANSFORM, true),
position(pos), rotation(rotation), scale(scale)
{
	SetLocalTransformation(position, rotation, scale);
	GenerateGlobalTransformationFrom(parentTransform);
}

C_Transform::C_Transform(float4x4 parentTransform, float4x4 localTransform) : Component(COMPONENT_TYPE::TRANSFORM, true), localTransform(localTransform), globalTransform(float4x4::identity)
{
	SetLocalTransformation(localTransform);
	GenerateGlobalTransformationFrom(parentTransform);
}

C_Transform::~C_Transform()
{}

void C_Transform::Update(float dt)
{}

void C_Transform::SetLocalTransformation(float4x4 transform)
{
	transform.Decompose(position, rotation, scale);
	GenerateEulerFromRot();
}

void C_Transform::SetLocalTransformation(float3 pos, Quat rot, float3 scl)
{
	this->position = pos;
	this->rotation = rot;
	this->scale = scl;
	localTransform = float4x4::FromTRS(position, rotation, scale);
	GenerateEulerFromRot();
}

void C_Transform::GenerateGlobalTransformationFrom(float4x4 parentTransform)
{
	globalTransform = parentTransform * localTransform;
	tGlobalTransform = globalTransform.Transposed();
}

void C_Transform::SetGlobalTransformation(float4x4 transform)
{
	globalTransform = transform;
}

float4x4 C_Transform::GetLocalTransform() const
{
	return localTransform;
}

float4x4 C_Transform::GetGlobalTransform() const
{
	return globalTransform;
}

float4x4 C_Transform::GetTGlobalTransform() const
{
	return tGlobalTransform;
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
