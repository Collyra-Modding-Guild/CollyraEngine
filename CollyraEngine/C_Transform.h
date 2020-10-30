#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class C_Transform : public Component
{
public:

	C_Transform();
	C_Transform(float4x4 parentTransform, float3 pos, Quat rotation, float3 scale);
	C_Transform(float4x4 parentTransform, float4x4 localTransform);
	~C_Transform();

	//Setters-------------------
	void SetLocalTransformation(float4x4 transform);
	void SetLocalTransformation(float3 pos, Quat rotation, float3 scale);
	void GenerateGlobalTransformationFrom(float4x4 parentTransform);
	void SetGlobalTransformation(float4x4 globalTransform);

	//Getters-------------------
	float4x4	GetLocalTransform() const;
	float4x4	GetGlobalTransform() const;
	float4x4	GetTGlobalTransform() const;
	float3		GetPosition() const;
	Quat		GetRotation() const;
	float3		GetRotationEuler() const;
	float3		GetScale() const;
	float3		GetGlobalScale() const;

private:
	void GenerateEulerFromRot();

private:
	float4x4 localTransform;
	float4x4 globalTransform;
	float4x4 tGlobalTransform;

	float3 position;
	Quat rotation;
	float3 scale;

	float3 rotationEuler;

};