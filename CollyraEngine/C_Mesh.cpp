#include "C_Mesh.h"

C_Mesh::C_Mesh(bool active) : myMesh(nullptr), Component(COMPONENT_TYPE::MESH, active)
{
}

C_Mesh::~C_Mesh()
{
}

void C_Mesh::Update(float dt)
{
}
