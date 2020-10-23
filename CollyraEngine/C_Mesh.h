#pragma once

#include "Component.h"
#include "Mesh.h"

class C_Mesh : public Component
{
public:
	C_Mesh();
	~C_Mesh();

	void Update() override;

private:

	Mesh* myMesh;

};