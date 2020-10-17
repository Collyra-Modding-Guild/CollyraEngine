#ifndef __Mesh_H__
#define __Mesh_H__

#include "Globals.h"
#include "Color.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float2.h"
#include <vector>

struct Vertex 
{
	float3 Position;
	float3 Normal;
	float2 TexCoords;
};

class Mesh
{
public:
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<uint> indices);
	~Mesh();

	void GenerateBuffers();

	void Render(bool globalWireMode = false) const;
	void InnerRender() const;

public:
	uint idVertex; // unique vertex in VRAM
	std::vector<Vertex>       vertices;

	uint idIndex; // index in VRAM
	std::vector<uint> indices;

	uint VAO;

private:
	Color color;
	bool wire, noFace;

};

#endif