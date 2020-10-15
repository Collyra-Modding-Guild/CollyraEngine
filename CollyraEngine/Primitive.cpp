#include "Globals.h"
#include "Primitive.h"

#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

uint cubeIndices[] =
{
	0, 1, 2,
	2, 3, 0,

	1, 5, 6,
	6, 2, 1,

	7, 6, 5,
	5, 4, 7,

	4, 0, 3,
	3, 7, 4,

	4, 5, 1,
	1, 0, 4,

	3, 2, 6,
	6, 7, 3
};


// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point), isInvisible(false), verticesID(0), indicesID(0), indicesSize(-1), vertices(nullptr), indices(nullptr)
{}

Primitive::Primitive(GLfloat vertices[], uint indices[]) : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point), isInvisible(false), verticesID(0), indicesID(0), indicesSize(-1)
{
	GenerateVertexBuffers(vertices, indices);
}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render(bool globalDebugMode) const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if (wire && !globalDebugMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}

	InnerRender();

	if (wire && !globalDebugMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3& u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
CCube::CCube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
	GenerateCubeVertices(sizeX, sizeY, sizeZ);
	GenerateCubeIndices();
}

void CCube::GenerateCubeVertices(float sizeX, float sizeY, float sizeZ)
{
	GLfloat vertices[] =
	{
	-sizeX, -sizeY, sizeZ,
	sizeX, -sizeY, sizeZ,
	sizeX, sizeY, sizeZ ,
	-sizeX, sizeY, sizeZ,

	-sizeX, -sizeY, -sizeZ,
	sizeX, -sizeY, -sizeZ,
	sizeX, sizeY, -sizeZ,
	-sizeX, sizeY, -sizeZ
	};
	glGenBuffers(1, (GLuint*)&(verticesID));
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void CCube::GenerateCubeIndices()
{
	glGenBuffers(1, (GLuint*)&(indicesID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	indicesSize = sizeof(cubeIndices) / sizeof(uint);
}


// SPHERE ============================================
SSphere::SSphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

SSphere::SSphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

SSphere::SSphere(float radius, int sectors, int stacks) : Primitive(), radius(radius)
{
	GenerateSphereVertices(radius, sectors, stacks);
	GenerateSphereIndices(sectors, stacks);
}

void SSphere::GenerateSphereVertices(float radius, int sectors, int stacks)
{

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal

	float sectorStep = 2 * PI / sectors;
	float stackStep = PI / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);
		}
	}

	glGenBuffers(1, (GLuint*) & (verticesID));
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	int verticeSize = vertices.size() * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, verticeSize, &vertices[0], GL_STATIC_DRAW);
}

void SSphere::GenerateSphereIndices(int sectors, int stacks)
{
	// generate CCW index list of sphere triangles
	std::vector<int> indices;
	int k1, k2;
	for (int i = 0; i < sectors; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	glGenBuffers(1, (GLuint*) & (indicesID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	int indi = indices.size() * sizeof(float);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indi, &indices[0], GL_STATIC_DRAW);
	indicesSize = indices.size();
}



// CYLINDER ============================================
CCylinder::CCylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

CCylinder::CCylinder(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

CCylinder::CCylinder(float radius, int sectors, int height) : Primitive(), radius(radius), height(height)
{
	GenerateCylinderVertices(radius, sectors, height);
	GenerateCylinderIndices(sectors);
}

void CCylinder::GenerateCylinderVertices(float radius, int sectors, int height)
{

	float sectorStep = 2 * PI / sectors;
	float sectorAngle;  // radian

	std::vector<float> unitVertices;
	for (int i = 0; i <= sectors; ++i)
	{
		sectorAngle = i * sectorStep;
		unitVertices.push_back(cos(sectorAngle)); // x
		unitVertices.push_back(sin(sectorAngle)); // y
		unitVertices.push_back(0);                // z
	}

	std::vector<float> vertices;
	std::vector<float> normals;

	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		for (int j = 0, k = 0; j <= sectors; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			float uz = unitVertices[k + 2];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(uy * radius);             // vy
			vertices.push_back(h);                       // vz
			// normal vector
			normals.push_back(ux);                       // nx
			normals.push_back(uy);                       // ny
			normals.push_back(uz);                       // nz
		}
	}

	// the starting index for the base/top surface
	//NOTE: it is used for generating indices later
	baseCenterIndex = (int)vertices.size() / 3;
	topCenterIndex = baseCenterIndex + sectors + 1; // include center vertex

	 // put base and top vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float nz = -1 + i * 2;                           // z value of normal; -1 to 1

		// center point
		vertices.push_back(0);     vertices.push_back(0);     vertices.push_back(h);
		normals.push_back(0);      normals.push_back(0);      normals.push_back(nz);

		for (int j = 0, k = 0; j < sectors; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(uy * radius);             // vy
			vertices.push_back(h);                       // vz
			// normal vector
			normals.push_back(0);                        // nx
			normals.push_back(0);                        // ny
			normals.push_back(nz);                       // nz
		}
	}

	glGenBuffers(1, (GLuint*) & (verticesID));
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	int verticeSize = vertices.size() * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, verticeSize, &vertices[0], GL_STATIC_DRAW);
}

void CCylinder::GenerateCylinderIndices(int sectors)
{

	// generate CCW index list of cylinder triangles
	std::vector<int> indices;
	int k1 = 0;                         // 1st vertex index at base
	int k2 = sectors + 1;           // 1st vertex index at top

	// indices for the side surface
	for (int i = 0; i < sectors; ++i, ++k1, ++k2)
	{
		// 2 triangles per sector
		// k1 => k1+1 => k2
		indices.push_back(k1);
		indices.push_back(k1 + 1);
		indices.push_back(k2);

		// k2 => k1+1 => k2+1
		indices.push_back(k2);
		indices.push_back(k1 + 1);
		indices.push_back(k2 + 1);
	}

	// indices for the base surface
	//NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
	//      please see the previous code snippet
	for (int i = 0, k = baseCenterIndex + 1; i < sectors; ++i, ++k)
	{
		if (i < sectors - 1)
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(k + 1);
			indices.push_back(k);
		}
		else // last triangle
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(baseCenterIndex + 1);
			indices.push_back(k);
		}
	}

	// indices for the top surface
	for (int i = 0, k = topCenterIndex + 1; i < sectors; ++i, ++k)
	{
		if (i < sectors - 1)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(k + 1);
		}
		else // last triangle
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(topCenterIndex + 1);
		}
	}

	glGenBuffers(1, (GLuint*) & (indicesID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	int indi = indices.size() * sizeof(float);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indi, &indices[0], GL_STATIC_DRAW);
	indicesSize = indices.size();
}

// LINE ==================================================
Line::Line() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

Line::Line(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
CPlane::CPlane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

CPlane::CPlane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void CPlane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}


// ---------------------------------------------------------
void Primitive::SetInvisible(bool isInvisible)
{
	if (this->isInvisible != isInvisible)
	{
		this->isInvisible = isInvisible;
	}
}

void Primitive::GenerateVertexBuffers(GLfloat* vertices, uint* indices)
{
	glGenBuffers(1, (GLuint*)&(verticesID));
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&(indicesID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*indices), indices, GL_STATIC_DRAW);
	indicesSize = sizeof(*indices) / sizeof(uint);
}