#pragma once

#include "Component.h"

class C_Material : public Component
{
public:

	C_Material(bool active);
	~C_Material();

	void Update(float dt) override;

private:

};