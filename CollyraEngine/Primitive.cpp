#include "Globals.h"
#include "Primitive.h"
#include "PrimitivesIndices.h"

#include "OpenGL.h"


// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point), verticesID(0), indicesID(0), indicesSize(-1)
{}

Primitive::~Primitive()
{
	glDeleteBuffers(1, &verticesID);
	glDeleteBuffers(1, &indicesID);
}

Primitive::Primitive(GLfloat vertices[], uint indices[]) : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point), verticesID(0), indicesID(0), indicesSize(-1)
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
	type = PrimitiveTypes::Primitive_Sphere;
	GenerateSphereVertices(radius, sectors, stacks);
	GenerateSphereIndices(sectors, stacks);
}

void SSphere::GenerateSphereVertices(float radius, int sectors, int stacks)
{
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;

	float x, y, z, xy;                              
	float nx, ny, nz, lengthInv = 1.0f / radius;    
	float s, t;

	float sectorStep = 2 * PI / sectors;
	float stackStep = PI / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        
		xy = radius * cosf(stackAngle);            
		z = radius * sinf(stackAngle);              


		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;         

			x = xy * cosf(sectorAngle);            
			y = xy * sinf(sectorAngle);           
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);


			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);


			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			texCoords.push_back(s);
			texCoords.push_back(t);
		}
	}

	glGenBuffers(1, (GLuint*)&(verticesID));
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	int verticeSize = vertices.size() * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, verticeSize, &vertices[0], GL_STATIC_DRAW);
}

void SSphere::GenerateSphereIndices(int sectors, int stacks)
{
	std::vector<int> indices;
	int k1, k2;
	for (int i = 0; i < sectors; ++i)
	{
		k1 = i * (sectors + 1);   
		k2 = k1 + sectors + 1;     

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{

			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stacks - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	glGenBuffers(1, (GLuint*)&(indicesID));
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
	type = PrimitiveTypes::Primitive_Cylinder;

	GenerateCylinderVertices(radius, sectors, height);
	GenerateCylinderIndices(sectors);
}

void CCylinder::GenerateCylinderVertices(float radius, int sectors, int height)
{
	float sectorStep = 2 * PI / sectors;
	float sectorAngle;

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
	std::vector<float> texCoords;

	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;
		float t = 1.0f - i;

		for (int j = 0, k = 0; j <= sectors; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			float uz = unitVertices[k + 2];


			vertices.push_back(ux * radius);
			vertices.push_back(uy * radius);
			vertices.push_back(h);

			normals.push_back(ux);
			normals.push_back(uy);
			normals.push_back(uz);

			texCoords.push_back((float)j / sectors);
			texCoords.push_back(t);                      
		}


		baseCenterIndex = (int)vertices.size() / 3;
		topCenterIndex = baseCenterIndex + sectors + 1;

		for (int i = 0; i < 2; ++i)
		{
			float h = -height / 2.0f + i * height;
			float nz = -1 + i * 2;

			vertices.push_back(0);     vertices.push_back(0);     vertices.push_back(h);
			normals.push_back(0);      normals.push_back(0);      normals.push_back(nz);

			for (int j = 0, k = 0; j < sectors; ++j, k += 3)
			{
				float ux = unitVertices[k];
				float uy = unitVertices[k + 1];

				// position vector
				vertices.push_back(ux * radius);
				vertices.push_back(uy * radius);
				vertices.push_back(h);

				// normal vector
				normals.push_back(0);
				normals.push_back(0);
				normals.push_back(nz);
			}
		}

		glGenBuffers(1, (GLuint*) & (verticesID));
		glBindBuffer(GL_ARRAY_BUFFER, verticesID);
		int verticeSize = vertices.size() * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, verticeSize, &vertices[0], GL_STATIC_DRAW);
	}
}

void CCylinder::GenerateCylinderIndices(int sectors)
{
	std::vector<int> indices;
	int k1 = 0;                   
	int k2 = sectors + 1;    

	for (int i = 0; i < sectors; ++i, ++k1, ++k2)
	{
		indices.push_back(k1);
		indices.push_back(k1 + 1);
		indices.push_back(k2);

		indices.push_back(k2);
		indices.push_back(k1 + 1);
		indices.push_back(k2 + 1);
	}

	for (int i = 0, k = baseCenterIndex + 1; i < sectors; ++i, ++k)
	{
		if (i < sectors - 1)
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(k + 1);
			indices.push_back(k);
		}
		else 
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(baseCenterIndex + 1);
			indices.push_back(k);
		}
	}

	for (int i = 0, k = topCenterIndex + 1; i < sectors; ++i, ++k)
	{
		if (i < sectors - 1)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(k + 1);
		}
		else 
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(topCenterIndex + 1);
		}
	}

	glGenBuffers(1, (GLuint*)&(indicesID));
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

// PYRAMID ============================================
Pyramid::Pyramid(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Pyramid;
	GeneratePyramidVertices(sizeX, sizeY, sizeZ);
	GeneratePyramidIndices();
}

void Pyramid::GeneratePyramidVertices(float sizeX, float sizeY, float sizeZ)
{
	GLfloat vertices[] =
	{
	0.0f, sizeY, 0.0f, 

	-sizeX, -sizeY, -sizeZ,
	sizeX, -sizeY, -sizeZ,
	sizeX, -sizeY, sizeZ,
	-sizeX, -sizeY, sizeZ
	};

	glGenBuffers(1, (GLuint*)&(verticesID));
	glBindBuffer(GL_ARRAY_BUFFER, verticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void Pyramid::GeneratePyramidIndices()
{
	glGenBuffers(1, (GLuint*)&(indicesID));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);
	indicesSize = sizeof(pyramidIndices) / sizeof(uint);
}
