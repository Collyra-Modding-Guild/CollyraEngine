#pragma once
#include "glmath.h"
#include "Color.h"
#include <vector>


#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>



enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Sensor
};

typedef float GLfloat;

class Primitive
{
public:

	Primitive();
	Primitive(GLfloat vertices[], uint indices[]);

	virtual void	Render(bool globalDebugMode = false) const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	void			SetInvisible(bool inVisible);

	void			GenerateVertexBuffers(GLfloat* vertices, uint* indices);

	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;
	bool isInvisible;

	float* vertices;
	uint* indices;

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
	vec3 size;
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
class Line : public Primitive
{
public:
	Line();
	Line(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class CPlane : public Primitive
{
public:
	CPlane();
	CPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};