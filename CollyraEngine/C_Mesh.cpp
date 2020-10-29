#include "C_Mesh.h"

#include "OpenGL.h"

C_Mesh::C_Mesh(bool active) : Component(COMPONENT_TYPE::MESH, active), idIndex(-1), idVertex(-1), idNormals(-1), idTextureCoords(-1),
wire(false), noFace(false)
{}

C_Mesh::C_Mesh(std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords, bool active) :
	Component(COMPONENT_TYPE::MESH, active),
	idIndex(-1), idVertex(-1), idNormals(-1), idTextureCoords(-1),
	wire(false), noFace(false)
{}

C_Mesh::~C_Mesh()
{
	ClearMesh();
}

void C_Mesh::GenerateMesh(const char* meshName, const char* meshPath, std::vector<float3> vertices, std::vector<uint> indices, std::vector<float3> normals, std::vector<float2> textureCoords)
{
	this->vertices = vertices;
	this->indices = indices;
	this->normals = normals;
	this->textureCoords = textureCoords;

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
void C_Mesh::Render(float4x4 transform, int textureID, Color color, bool globalWireMode) const
{
	glPushMatrix();
	glMultMatrixf((float*)&transform);

	if (globalWireMode)
		glColor3f(wireColor.r, wireColor.g, wireColor.b);
	else
		glColor3f(color.r, color.g, color.b);

	InnerRender(textureID);

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

void C_Mesh::DrawNormals() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	uint lineLength = 5;

	if (vertices.size() == normals.size())
		for (uint i = 0, j = 0; i < vertices.size(); i++)
		{
			//Draw Vertex Normals-----------------------
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

			float3 vector = vertices[i];
			float3 normal = vector + normals[i] * lineLength;

			glVertex3f(vector.x, vector.y, vector.z); glVertex3f(normal.x, normal.y, normal.z);

			glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
			j++;

			//Draw Faces normals-------------------
			if (j == 3)
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