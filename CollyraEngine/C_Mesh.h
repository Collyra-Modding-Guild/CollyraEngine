#pragma once

#include "Component.h"
#include "Mesh.h"

class C_Mesh : public Component
{
public:
	C_Mesh(bool active);
	~C_Mesh();

	void Update(float dt) override;

private:

	Mesh* myMesh;

};