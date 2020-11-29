#ifndef __Resource_Mesh_H__
#define __Resource_Mesh_H__

#include "R_Resource.h"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "Globals.h"
#include "Color.h"

class R_Mesh : public Resource
{
public:
	R_Mesh(unsigned int uid);
	virtual ~R_Mesh();

	void GenerateMesh(std::vector<float3> vertices = {}, std::vector<uint> indices = {}, std::vector<float3> normals = {}, std::vector<float2> textureCoords = {});
	void SetAABB();

	uint GetVerticesSize() const;
	std::vector<float3>* GetVertices();
	float3* GetVerticesIndex();
	uint GetVerticesId() const;

	std::vector<uint>* GetIndices();
	uint GetIndicesSize() const;
	uint* GetIndicesIndex();
	uint GetIndicesId() const;

	std::vector<float3>* GetNormals();
	uint GetNormalsSize() const;
	float3* GetNormalsIndex();
	float GetNormalsId() const;

	std::vector<float2>* GetTextCoords();
	uint GetTextureCoordsSize() const;
	float2* GetTextureCoordsIndex();
	float GetTextureCoordsId() const;

	AABB GetAABB() const;

	Color GetWireColor() const;

private:
	void GenerateBuffers();
	void GenerateWireColor();
	void ClearMesh();

public:

	std::vector<float3> vertices;
	const float3 verticesPoint;

	std::vector<uint> indices;

	std::vector<float3> normals;
	std::vector<float2> textureCoords;


private:

	uint idIndex;
	uint idNormals;
	uint idVertex;
	uint idTextureCoords;

	Color wireColor;

	AABB aabb;

};

#endif //__Resource_Mesh_H__