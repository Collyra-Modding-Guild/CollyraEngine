#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class C_Transform : public Component
{
public:

	C_Transform();
	~C_Transform();

	void Update(float dt) override;

	float4x4 GetTransform() const;

private:
	float4x4 transform;


};