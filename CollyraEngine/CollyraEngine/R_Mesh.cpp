#include "R_Mesh.h"
#include "OpenGL.h"

R_Mesh::R_Mesh(unsigned int uid) : Resource(uid, R_TYPE::MESH), idIndex(0), idVertex(0), idNormals(0), idTextureCoords(0),
 vertices{}, indices{}, normals{}, textureCoords{}
{}


R_Mesh::~R_Mesh()
{
	ClearMesh();
}

void R_Mesh::GenerateMesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords)
{
	if (vertices.size() > 0)
		this->vertices = vertices;
	if (indices.size() > 0)
		this->indices = indices;
	if (normals.size() > 0)
		this->normals = normals;
	if (textureCoords.size() > 0)
		this->textureCoords = textureCoords;

	SetAABB();

	GenerateBuffers();
	GenerateWireColor();
}

void R_Mesh::SetAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose(&vertices[0], (uint)vertices.size());
}

void R_Mesh::GenerateBuffers()
{
	if (!indices.empty() && !vertices.empty())
	{
		glGenBuffers(1, (GLuint*)&(idVertex));
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float3), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&(idIndex));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

		if (!normals.empty())
		{
			glGenBuffers(1, (GLuint*)&(idNormals));
			glBindBuffer(GL_ARRAY_BUFFER, idNormals);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float3), &normals[0], GL_STATIC_DRAW);
		}

		if (!textureCoords.empty())
		{
			glGenBuffers(1, (GLuint*)&(idTextureCoords));
			glBindBuffer(GL_ARRAY_BUFFER, idTextureCoords);
			glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(float2), &textureCoords[0], GL_STATIC_DRAW);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
}

void R_Mesh::GenerateWireColor()
{
	wireColor = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
}

void R_Mesh::ClearMesh()
{
	vertices.clear();
	indices.clear();
	normals.clear();
	textureCoords.clear();

	glDeleteBuffers(1, &idVertex);
	glDeleteBuffers(1, &idIndex);
	glDeleteBuffers(1, &idNormals);
	glDeleteBuffers(1, &idTextureCoords);

	idVertex = 0;
	idIndex = 0;
	idNormals = 0;
	idTextureCoords = 0;
}


uint R_Mesh::GetVerticesSize() const
{
	return vertices.size();
}

std::vector<float3>* R_Mesh::GetVertices()
{
	return &vertices;
}

float3* R_Mesh::GetVerticesIndex()
{
	return &vertices[0];
}

uint R_Mesh::GetVerticesId() const
{
	return idVertex;
}

std::vector<uint>* R_Mesh::GetIndices()
{
	return &indices;
}

uint R_Mesh::GetIndicesSize() const
{
	return indices.size();
}

uint* R_Mesh::GetIndicesIndex()
{
	return &indices[0];
}

uint R_Mesh::GetIndicesId() const
{
	return idIndex;
}

std::vector<float3>* R_Mesh::GetNormals()
{
	return &normals;
}

uint R_Mesh::GetNormalsSize() const
{
	return normals.size();
}

float3* R_Mesh::GetNormalsIndex()
{
	return &normals[0];
}

float R_Mesh::GetNormalsId() const
{
	return idNormals;
}

std::vector<float2>* R_Mesh::GetTextCoords()
{
	return &textureCoords;
}

uint R_Mesh::GetTextureCoordsSize() const
{
	return textureCoords.size();
}

float2* R_Mesh::GetTextureCoordsIndex()
{
	return &textureCoords[0];
}

float R_Mesh::GetTextureCoordsId() const
{
	return idTextureCoords;
}

AABB R_Mesh::GetAABB() const
{
	return aabb;
}

Color R_Mesh::GetWireColor() const
{
	return wireColor;
}
