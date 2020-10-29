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

	void Render(bool* drawState,float4x4 transform = float4x4::identity, int textureID = -1, Color color = Grey) const;
	void InnerRender(int textureID) const;

	void DrawNormals(bool* drawState) const;

	std::string GetMeshName() const;
	std::string GetMeshPath() const;

	uint GetVerticesSize() const;
	uint GetIndicesSize() const;
	uint GetNormalsSize() const;
	uint GetTextureCoordsSize() const;

	bool GetDrawingWire() const;
	void SetDrawingWire(bool newState);
	bool GetDrawingFaces() const;
	void SetDrawingFaces(bool newState);
	bool GetDrawingNormVertices() const;
	void SetDrawingNormVertices(bool newState);
	bool GetDrawingNormFaces() const;
	void SetDrawingNormFaces(bool newState);

	float3 GetSize() const;

private:
	void GenerateBuffers();
	void GenerateWireColor();
	void GenerateSize(float3 scale);

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
	bool drawWire, drawFaces, drawNormVertices, drawNormFaces;

	std::string name;
	std::string path;

	float3 meshSize;
};