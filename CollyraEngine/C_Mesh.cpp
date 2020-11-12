#include "C_Mesh.h"
#include "Globals.h"

#include "C_Transform.h"

#include "OpenGL.h"

C_Mesh::C_Mesh(bool active) : Component(COMPONENT_TYPE::MESH, active), idIndex(-1), idVertex(-1), idNormals(-1), idTextureCoords(-1),
drawWire(false), drawNormVertices(false), drawNormFaces(false), drawFaces(true)
{}

C_Mesh::C_Mesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords, bool active) :
	Component(COMPONENT_TYPE::MESH, active),
	idIndex(-1), idVertex(-1), idNormals(-1), idTextureCoords(-1),
	drawWire(false), drawNormVertices(false), drawNormFaces(false), drawFaces(true)
{}

C_Mesh::~C_Mesh()
{
	ClearMesh();
}

AABB C_Mesh::GetAABB() const
{
	return aabb;
}

void C_Mesh::SetAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose(&vertices[0], (uint)vertices.size());
}

void C_Mesh::GenerateMesh(const char* meshName, const char* meshPath, std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords)
{
	this->vertices = vertices;
	this->indices = indices;
	this->normals = normals;
	this->textureCoords = textureCoords;

	SetAABB();

	GenerateBuffers();
	GenerateWireColor();
	SetNameAndPath(meshName, meshPath);
}

void C_Mesh::SetNameAndPath(const char* meshName, const char* meshPath)
{
	name = meshName;
	path = meshPath;
}

void C_Mesh::GenerateBuffers()
{
	if (!indices.empty() && !vertices.empty())
	{
		glGenBuffers(1, (GLuint*)&(idVertex));
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float3), &vertices[0], GL_STATIC_DRAW);

		GenerateSize(this->myGameObject->GetComponent<C_Transform>()->GetGlobalScale());

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

void C_Mesh::GenerateWireColor()
{
	wireColor = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
}

void C_Mesh::GenerateSize(float3 scale)
{
	float maxY = 0, maxX = 0, maxZ = 0;
	float minY = FLT_MAX, minX = FLT_MAX, minZ = FLT_MAX;

	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x > maxX)
		{
			maxX = vertices[i].x;
		}

		if (vertices[i].x < minX)
		{
			minX = vertices[i].x;
		}

		if (vertices[i].y > maxY)
		{
			maxY = vertices[i].y;
		}

		if (vertices[i].y < minY)
		{
			minY = vertices[i].y;
		}

		if (vertices[i].z > maxZ)
		{
			maxZ = vertices[i].z;
		}

		if (vertices[i].z < minZ)
		{
			minZ = vertices[i].z;
		}
	}

	meshSize.x = abs(maxX - minX) * scale.x;
	meshSize.y = abs(maxY - minY) * scale.y;
	meshSize.z = abs(maxZ - minZ) * scale.z;
}

void C_Mesh::ClearMesh()
{
	vertices.clear();
	indices.clear();
	normals.clear();
	textureCoords.clear();

	glDeleteBuffers(1, &idVertex);
	glDeleteBuffers(1, &idIndex);
	glDeleteBuffers(1, &idNormals);
	glDeleteBuffers(1, &idTextureCoords);
}


// ------------------------------------------------------------
void C_Mesh::Render(bool* drawState, float4x4 transform, int textureID, Color color) const
{
	glPushMatrix();
	glMultMatrixf((float*)&transform);

	if (drawWire || drawState[WIRE] == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glColor3f(wireColor.r, wireColor.g, wireColor.b);
		glLineWidth(2.0f);

		InnerRender(-1);

		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (drawFaces && drawState[FACES] != false)
	{
		glColor3f(color.r, color.g, color.b);
		InnerRender(textureID);
	}

	if (drawNormVertices || drawNormFaces || drawState[NORMAL_V] == true || drawState[NORMAL_F] == true)
		DrawNormals(drawState);

	glPopMatrix();
}

// ------------------------------------------------------------
void C_Mesh::InnerRender(int textureID) const
{
	glEnableClientState(GL_VERTEX_ARRAY);

	if (idNormals != -1)
		glEnableClientState(GL_NORMAL_ARRAY);

	if (idTextureCoords != -1)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (textureID != -1)
		glEnableClientState(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (idNormals != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, idNormals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (idTextureCoords != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, idTextureCoords);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	if (textureID != -1)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);

}

void C_Mesh::DrawNormals(bool* drawState) const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	uint lineLength = 1;

	if (vertices.size() == normals.size())
		for (uint i = 0, j = 0; i < vertices.size(); i++)
		{
			//Draw Vertex Normals-----------------------
			if (drawNormVertices == true|| drawState[NORMAL_V])
			{
				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);


				float3 vector = vertices[i];
				float3 normal = vector + normals[i] * lineLength;

				glVertex3f(vector.x, vector.y, vector.z); glVertex3f(normal.x, normal.y, normal.z);
			}

			glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
			j++;

			//Draw Faces normals-------------------
			if (j == 3 && (drawNormFaces == true || drawState[NORMAL_F]))
			{
				float3 P0 = vertices[i - 2];
				float3 P1 = vertices[i - 1];
				float3 P2 = vertices[i];

				float3 V0 = P0 - P1;
				float3 V1 = P2 - P1;

				//Normal of the face
				float3 N = V1.Cross(V0);
				N.Normalize();

				// Center of the triangle
				float3 P = (P0 + P1 + P2) / 3.0;

				float3 normal = P + N * lineLength;

				glVertex3f(P.x, P.y, P.z); glVertex3f(normal.x, normal.y, normal.z);

				j = 0;
			}
		}

	glEnd();

	glLineWidth(1.0f);
}

uint C_Mesh::GetVerticesSize() const
{
	return vertices.size();
}

uint C_Mesh::GetIndicesSize() const
{
	return indices.size();
}

uint C_Mesh::GetNormalsSize() const
{
	return normals.size();
}

uint C_Mesh::GetTextureCoordsSize() const
{
	return textureCoords.size();
}

bool C_Mesh::GetDrawingWire() const
{
	return drawWire;
}

void C_Mesh::SetDrawingWire(bool newState)
{
	drawWire = newState;
}

bool C_Mesh::GetDrawingFaces() const
{
	return drawFaces;
}

void C_Mesh::SetDrawingFaces(bool newState)
{
	drawFaces = newState;
}

bool C_Mesh::GetDrawingNormVertices() const
{
	return drawNormVertices;
}

void C_Mesh::SetDrawingNormVertices(bool newState)
{
	drawNormVertices = newState;
}

bool C_Mesh::GetDrawingNormFaces() const
{
	return drawNormFaces;
}

void C_Mesh::SetDrawingNormFaces(bool newState)
{
	drawNormFaces = newState;
}

float3 C_Mesh::GetSize() const
{
	return meshSize;
}

std::string C_Mesh::GetMeshName() const
{
	return name;
}

std::string C_Mesh::GetMeshPath() const
{
	return path;
}
