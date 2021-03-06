#include "C_Transform.h"
#include "Globals.h"

C_Transform::C_Transform() : Component(COMPONENT_TYPE::TRANSFORM, true), localTransform(float4x4::identity), globalTransform(float4x4::identity),
position(0, 0, 0), rotation(Quat::identity), scale(1, 1, 1), tGlobalTransform(float4x4::identity), hasUpdated(false), globlPos(0,0,0)
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

	Quat gloRot;
	this->myGameObject->GetParent()->GetComponent<C_Transform>()->globalTransform.Decompose(float3(), gloRot, float3());

	forward = { 2 * (rotation.x * rotation.z + rotation.w * rotation.y),
	2 * (rotation.y * rotation.z - rotation.w * rotation.x),
	1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y) };

	forward = gloRot * forward;

}

void C_Transform::SetLocalTransformation(float3 pos, Quat rot, float3 scale)
{
	this->position = pos;
	this->rotation = rot;
	this->scale = scale;
	localTransform = float4x4::FromTRS(position, rotation, scale);
	hasUpdated = true;
	GenerateEulerFromRot();

	Quat gloRot;
	this->myGameObject->GetParent()->GetComponent<C_Transform>()->globalTransform.Decompose(float3(), gloRot, float3());

	forward = { 2 * (rotation.x * rotation.z + rotation.w * rotation.y),
	2 * (rotation.y * rotation.z - rotation.w * rotation.x),
	1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y) };

	forward = gloRot * forward;
}

//Transposed because OpenGL <=> MathGeoLib communication
void C_Transform::GenerateGlobalTransformationFrom(float4x4 parentTransform)
{
	globalTransform = parentTransform * localTransform;
	tGlobalTransform = globalTransform.Transposed();

	globalTransform.Decompose(globlPos, Quat(), float3());

	Quat gloRot;
	parentTransform.Decompose(float3(), gloRot, float3());

	forward = { 2 * (rotation.x * rotation.z + rotation.w * rotation.y),
	2 * (rotation.y * rotation.z - rotation.w * rotation.x),
	1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y) };

	forward = gloRot * forward;

}

void C_Transform::UpdateGuizmoTransformation(float4x4 transform, float4x4 parentTransform)
{
	SetLocalTransformation(parentTransform.Inverted() * transform);
	hasUpdated = true;
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

float3 C_Transform::GetGlobalPos() const
{
	return globlPos;
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

float3 C_Transform::GetForward() const
{
	return  forward;
}

void C_Transform::GenerateEulerFromRot()
{
	rotationEuler = rotation.ToEulerXYZ();
	rotationEuler *= RADTODEG;
}
