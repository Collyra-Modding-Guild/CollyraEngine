#pragma once

#include "Component.h"
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"

struct GameObject;

class C_Camera : public Component
{
public:

	C_Camera(bool active = true);
	~C_Camera();

	void UpdateFrustum(const float4x4& globalPosition);

private:


public:

	Frustum frustum;
};