#pragma once

#include "Component.h"
#include "Globals.h"
#include "Color.h"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"

class R_Mesh;

class __declspec(dllexport) C_Mesh : public Component
{
public:
	C_Mesh(bool active = true);
	~C_Mesh();

	AABB GetAABB() const;

	void Render(bool* drawState,float4x4 transform = float4x4::identity, int textureID = -1, Color color = Grey) const;
	void InnerRender(int textureID) const;

	void DrawNormals(bool* drawState) const;

	void DrawOutline(float4x4& transform) const;

	uint GetVerticesSize() const;
	std::vector<float3>* GetVertices();
	float3* GetVerticesIndex();

	std::vector<uint>* GetIndices();
	uint GetIndicesSize() const;
	uint* GetIndicesIndex();

	std::vector<float3>* GetNormals();
	uint GetNormalsSize() const;
	float3* GetNormalsIndex();

	std::vector<float2>* GetTextCoords();
	uint GetTextureCoordsSize() const;
	float2* GetTextureCoordsIndex();

	bool GetDrawingWire() const;
	void SetDrawingWire(bool newState);
	bool GetDrawingFaces() const;
	void SetDrawingFaces(bool newState);
	bool GetDrawingNormVertices() const;
	void SetDrawingNormVertices(bool newState);
	bool GetDrawingNormFaces() const;
	void SetDrawingNormFaces(bool newState);

	void SetResourceId(uint newId);
	void SetResourceIdFromName(const char* name);
	int GetResourceId();

	std::string GetName() const;
	std::string GetPath() const;
	
private:

	bool drawWire, drawFaces, drawNormVertices, drawNormFaces;

	uint rMeshId;
	R_Mesh* rMesh;
};