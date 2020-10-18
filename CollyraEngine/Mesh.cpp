#include "Mesh.h"
#include "p2Defs.h"


#include "Glew/include/glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib")

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

Mesh::Mesh() : idIndex(-1), idVertex(-1), wire(false), VAO(-1), noFace(false)
{
	color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices)
{
	this->vertices = vertices;
	this->indices = indices;

	GenerateBuffers();
	color = Color((float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f, (float)(std::rand() % 255) / 255.f);
}

Mesh::~Mesh()
{
	vertices.clear();
	indices.clear();
}

void Mesh::GenerateBuffers()
{
	if (!indices.empty() && !vertices.empty())
	{
		glGenBuffers(1, (GLuint*)&(idVertex));
		glBindBuffer(GL_ARRAY_BUFFER, idVertex);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, (GLuint*)&(idIndex));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);
	}
}

// ------------------------------------------------------------
void Mesh::Render(bool globalWireMode) const
{
	glPushMatrix();

	glColor3f(color.r, color.g, color.b);

	InnerRender();

	glLineWidth(2.0f);

	glBegin(GL_LINES);
	for (uint i = 0,j = 0; i < vertices.size(); i++)
	{
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		float3 vector = vertices[i].Position;
		float3 normals = vector + vertices[i].Normal * 2;

		glVertex3f(vector.x, vector.y, vector.z); glVertex3f(normals.x, normals.y, normals.z);

		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		j++;

		if (j == 3)
		{
			float3 P0 = vertices[i - 2].Position;
			float3 P1 = vertices[i - 1].Position;
			float3 P2 = vertices[i].Position;

			float3 V0 = P0 - P1;
			float3 V1 = P2 - P1;

			float3 N = V1.Cross(V0);
			N.Normalize();

			// Center of the triangle
			float3 P = (P0 + P1 + P2) / 3.0;

			float3 normal = P + N * 2;

			glVertex3f(P.x, P.y, P.z); glVertex3f(normal.x, normal.y, normal.z);

			j = 0;
		}


	}

	glEnd();

	glLineWidth(1.0f);


	glPopMatrix();
}

// ------------------------------------------------------------
void Mesh::InnerRender() const
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}
