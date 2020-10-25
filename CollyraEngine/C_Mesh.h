#pragma once

#include "Component.h"
#include "Globals.h"
#include "Color.h"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"

class C_Mesh : public Component
{
public:
	C_Mesh(bool active = true);
	C_Mesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords, bool active = true);
	C_Mesh(const C_Mesh& mesh);
	~C_Mesh();

	void GenerateMesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords);

	void SetTextureId(uint newId);

	void Render(bool globalWireMode = false) const;
	void InnerRender() const;

	void DrawNormals() const;

private:
	void GenerateBuffers();
	void GenerateColors();

	void ClearMesh();

private:

	uint idVertex;
	std::vector<float3> vertices;

	uint idIndex;
	std::vector<uint> indices;

	uint idNormals;
	std::vector<float3> normals;

	uint idTextureCoords;
	std::vector<float2> textureCoords;

	uint idTextureImage;


	Color color, wireColor;
	bool wire, noFace;

};