#ifndef __Mesh_H__
#define __Mesh_H__

#include "Globals.h"
#include "Color.h"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"

class Mesh
{
public:
	Mesh();
	Mesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords );
	Mesh(const Mesh& copy);
	~Mesh();

	void GenerateBuffers();
	void GenerateColors();

	void SetTextureId(uint newId);

	void Render(bool globalWireMode = false) const;
	void InnerRender() const;

	void DrawNormals() const;

public:
	uint idVertex; // unique vertex in VRAM
	std::vector<float3> vertices;

	uint idIndex; // index in VRAM
	std::vector<uint> indices;

	uint idNormals;
	std::vector<float3> normals;

	uint idTextureCoords;
	std::vector<float2> textureCoords;

	uint idTextureImage;

private:
	Color color, wireColor;
	bool wire, noFace;

};

#endif