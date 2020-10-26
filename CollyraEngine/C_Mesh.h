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

	void Render(float4x4 transform = float4x4::identity, uint textureID = NULL, Color color = { 156, 156, 156 }, bool globalWireMode = false) const;
	void InnerRender(uint textureID) const;

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


	Color wireColor;
	bool wire, noFace;

};