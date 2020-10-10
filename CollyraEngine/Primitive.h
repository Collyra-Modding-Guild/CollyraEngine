#pragma once
#include "glmath.h"
#include "Color.h"


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
class Sphere : public Primitive
{
public:
	Sphere();
	Sphere(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class Cylinder : public Primitive
{
public:
	Cylinder();
	Cylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
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