#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class C_Transform : public Component
{
public:

	C_Transform();
	C_Transform(float3 pos, Quat rotation, float3 scale);
	C_Transform(float4x4 transform);
	~C_Transform();

	void Update(float dt) override;

	void SetTransformation(float4x4 transform);
	void SetTransformation(float3 pos, Quat rotation, float3 scale);

	float4x4	GetTransform() const;
	float3		GetPosition() const;
	Quat		GetRotation() const;
	float3		GetRotationEuler() const;
	float3		GetScale() const;

private:
	void GenerateEulerFromRot();

private:
	float4x4 transform;

	float3 position;
	Quat rotation;
	float3 scale;

	float3 rotationEuler;

};