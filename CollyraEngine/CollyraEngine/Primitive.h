#pragma once
#include "Color.h"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Sensor,
	Primitive_Pyramid
};

typedef float GLfloat;

class Primitive
{
public:

	Primitive();
	virtual ~Primitive();
	Primitive(GLfloat vertices[], uint indices[]);

	virtual void	Render(bool globalDebugMode = false) const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const float3 &u);
	void			Scale(float x, float y, float z);

	void			GenerateVertexBuffers(GLfloat* vertices, uint* indices);

	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	float4x4 transform;
	bool axis,wire;

	uint verticesID, indicesID;
	int indicesSize;

protected:
	PrimitiveTypes type;
};

// ============================================
class CCube : public Primitive
{
public :
	CCube(float sizeX = 1.0f, float sizeY = 1.0f, float sizeZ = 1.0f);

	void			GenerateCubeVertices(float sizeX = 1.0f, float sizeY = 1.0f, float sizeZ = 1.0f);
	void			GenerateCubeIndices();

public:
	float3 size;

	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<float2> texCoords;

	std::vector<unsigned int> indices;
};

// ============================================
class SSphere : public Primitive
{
public:
	SSphere();
	SSphere(float radius);
	SSphere(float radius, int sectors, int stacks);

	void GenerateSphereVertices(float radius, int sectors, int stacks);
	void GenerateSphereIndices(int sectors, int stacks);

public:

	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<float2> texCoords;

	// memeber vars
	float radius;
	int sectorCount;                        // longitude, # of slices
	int stackCount;                         // latitude, # of stacks
	bool smooth;

	std::vector<unsigned int> indices;
	std::vector<unsigned int> lineIndices;

	// interleaved
	std::vector<float> interleavedVertices;
	int interleavedStride;                  // # of bytes to hop to the next vertex (should be 32 bytes)

};

// ============================================
class CCylinder : public Primitive
{
public:
	CCylinder();
	CCylinder(float radius, float height);
	CCylinder(float radius, int sectors, int height);

	void GenerateCylinderVertices(float radius, int sectors, int height);
	void GenerateCylinderIndices(int sectors);

public:

	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<float2> texCoords;

	// memeber vars
	float height;
	float radius;
	int sectors;                        // longitude, # of slices
	int stackCount;                         // latitude, # of stacks
	bool smooth;

	int baseCenterIndex;
	int topCenterIndex;

	std::vector<unsigned int> indices;
	std::vector<unsigned int> lineIndices;

	// interleaved
	std::vector<float> interleavedVertices;
	int interleavedStride;                  // # of bytes to hop to the next vertex (should be 32 bytes)
};

// ============================================
class Pyramid : public Primitive
{
public:
	Pyramid();
	Pyramid(int height);

	Pyramid(float sizeX = 1.0f, float sizeY = 1.0f, float sizeZ = 1.0f);

	void GeneratePyramidVertices(float sizeX = 1.0f, float sizeY = 1.0f, float sizeZ = 1.0f);
	void GeneratePyramidIndices();


public:
	float3 size;           // # of bytes to hop to the next vertex (should be 32 bytes)

	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<float2> texCoords;

	std::vector<unsigned int> indices;

};

// ============================================
class CPlane : public Primitive
{
public:
	CPlane();
	CPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	float3 normal;
	float constant;
};