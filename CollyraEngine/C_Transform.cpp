#include "C_Transform.h"
#include "Globals.h"

C_Transform::C_Transform() : Component(COMPONENT_TYPE::TRANSFORM, true), localTransform(float4x4::identity), globalTransform(float4x4::identity),
position(0, 0, 0), rotation(Quat::identity), scale(1, 1, 1), tGlobalTransform(float4x4::identity), hasUpdated(false)
{
	GenerateEulerFromRot();
}

C_Transform::C_Transform(float4x4 parentTransform, float3 pos, Quat rotation, float3 scale) : Component(COMPONENT_TYPE::TRANSFORM, true),
position(pos), rotation(rotation), scale(scale), hasUpdated(false)
{
	SetLocalTransformation(position, rotation, scale);
	GenerateGlobalTransformationFrom(parentTransform);
}

C_Transform::C_Transform(float4x4 parentTransform, float4x4 localTransform) : Component(COMPONENT_TYPE::TRANSFORM, true), localTransform(localTransform), globalTransform(float4x4::identity),
hasUpdated(false)
{
	SetLocalTransformation(localTransform);
	GenerateGlobalTransformationFrom(parentTransform);
}

C_Transform::~C_Transform()
{}

void C_Transform::SetLocalTransformation(float4x4 transform)
{
	transform.Decompose(position, rotation, scale);
	localTransform = transform;
	GenerateEulerFromRot();
	hasUpdated = true;
}

void C_Transform::SetLocalTransformation(float3 pos, Quat rot, float3 scl)
{
	this->position = pos;
	this->rotation = rot;
	this->scale = scl;
	localTransform = float4x4::FromTRS(position, rotation, scale);
	hasUpdated = true;
	GenerateEulerFromRot();
}

//Transposed because OpenGL <=> MathGeoLib communication
void C_Transform::GenerateGlobalTransformationFrom(float4x4 parentTransform)
{
	globalTransform = parentTransform * localTransform;
	tGlobalTransform = globalTransform.Transposed();
}

void C_Transform::SetGlobalTransformation(float4x4 transform)
{
	globalTransform = transform;
}

void C_Transform::UpdateLocalTransformMaintingGlobalToFit(float4x4 newParentTransform)
{
	SetLocalTransformation(newParentTransform.Inverted() * globalTransform);
	GenerateGlobalTransformationFrom(newParentTransform);
	hasUpdated = true;
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

float3 C_Transform::GetGlobalScale() const
{
	return globalTransform.GetScale();
}

void C_Transform::GenerateEulerFromRot()
{
	rotationEuler = rotation.ToEulerXYZ();
	rotationEuler *= RADTODEG;
}
