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

	void GenerateMesh(const char* meshName, const char* meshPath, std::vector<float3> vertices = {}, std::vector<uint> indices = {}, std::vector<float3> normals = {}, std::vector<float2> textureCoords = {});
	void SetNameAndPath(const char* meshName, const char* meshPath);
	void SetAABB();

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

private:
	void GenerateBuffers();
	void GenerateWireColor();
	void ClearMesh();

public:

	uint idVertex;
	std::vector<float3> vertices;
	const float3 verticesPoint;

	uint idIndex;
	std::vector<uint> indices;

private:

	uint idNormals;
	std::vector<float3> normals;

	uint idTextureCoords;
	std::vector<float2> textureCoords;

	Color wireColor;

	std::string name;
	std::string path;

	AABB aabb;

};

#endif //__Resource_Mesh_H__