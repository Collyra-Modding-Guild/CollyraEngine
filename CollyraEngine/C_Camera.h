#pragma once

#include "Component.h"
#include "Globals.h"
#include "GameObject.h"

class C_Camera : public Component
{
public:

	C_Camera(bool active = true);
	~C_Camera();

};