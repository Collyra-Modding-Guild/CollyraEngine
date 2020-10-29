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
	~C_Mesh();

	void GenerateMesh(const char* meshName, const char* meshPath,std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords);
	void SetNameAndPath(const char* meshName, const char* meshPath);

	void Render(float4x4 transform = float4x4::identity, int textureID = -1, Color color = Grey, bool globalWireMode = false) const;
	void InnerRender(int textureID) const;

	void DrawNormals() const;

private:
	void GenerateBuffers();
	void GenerateWireColor();

	void ClearMesh();

public:

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

	std::string name;
	std::string path;

};