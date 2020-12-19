#include "C_Mesh.h"
#include "Globals.h"
#include "C_Transform.h"

#include "Application.h"
#include "M_Resources.h"
#include "R_Mesh.h"

#include "OpenGL.h"

C_Mesh::C_Mesh(bool active) : Component(COMPONENT_TYPE::MESH, active),
drawWire(false), drawNormVertices(false), drawNormFaces(false), drawFaces(true), rMeshId(0), rMesh(nullptr)
{}

C_Mesh::~C_Mesh()
{
	App->resources->UnloadResource(rMeshId);
}

AABB C_Mesh::GetAABB() const
{
	return rMesh ? rMesh->GetAABB() : AABB();
}

//void C_Mesh::SetAABB()
//{
//	aabb.SetNegativeInfinity();
//	aabb.Enclose(&vertices[0], (uint)vertices.size());
//}

// ------------------------------------------------------------
void C_Mesh::Render(bool* drawState, float4x4 transform, int textureID, Color color) const
{
	if (rMesh == nullptr)
		return;

	glPushMatrix();
	glMultMatrixf((float*)&transform);

	if (drawWire || drawState[WIRE] == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		Color wireColor = rMesh->GetWireColor();
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);

	if (rMesh->GetNormalsId() != -1)
		glEnableClientState(GL_NORMAL_ARRAY);

	if (rMesh->GetTextureCoordsId() != -1)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (textureID != -1)
		glEnableClientState(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, rMesh->GetVerticesId());
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (rMesh->GetNormalsId() != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, rMesh->GetNormalsId());
		glNormalPointer(GL_FLOAT, 0, NULL);
	}

	if (rMesh->GetTextureCoordsId() != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, rMesh->GetTextureCoordsId());
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	if (textureID != -1)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rMesh->GetIndicesId());

	glDrawElements(GL_TRIANGLES, rMesh->GetIndicesSize(), GL_UNSIGNED_INT, NULL);

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
	if (rMesh == nullptr)
		return;

	glLineWidth(2.0f);

	glBegin(GL_LINES);

	uint lineLength = 1;

	if (rMesh->GetVerticesSize() == rMesh->GetNormalsSize())
		for (uint i = 0, j = 0; i < rMesh->GetVerticesSize(); i++)
		{
			//Draw Vertex Normals-----------------------
			if (drawNormVertices == true || drawState[NORMAL_V])
			{
				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);


				float3 vector = rMesh->vertices[i];
				float3 normal = vector + rMesh->normals[i] * lineLength;

				glVertex3f(vector.x, vector.y, vector.z); glVertex3f(normal.x, normal.y, normal.z);
			}

			glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
			j++;

			//Draw Faces normals-------------------
			if (j == 3 && (drawNormFaces == true || drawState[NORMAL_F]))
			{
				float3 P0 = rMesh->vertices[i - 2];
				float3 P1 = rMesh->vertices[i - 1];
				float3 P2 = rMesh->vertices[i];

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

void C_Mesh::DrawOutline(float4x4& transform) const
{
	if (rMesh == nullptr)
		return;

	if (rMesh->GetVertices() == nullptr || rMesh->GetIndices() == nullptr)
		return;

	glPushMatrix();
	glMultMatrixf((float*)&transform);

	glDisable(GL_LIGHTING);

	glLineWidth(10.f);
	// - - - - - - - - - -
	glPolygonMode(GL_FRONT, GL_LINE);
	glBindBuffer(GL_ARRAY_BUFFER, rMesh->GetVerticesId());
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rMesh->GetIndicesId());
	glDrawElements(GL_TRIANGLES, rMesh->GetIndicesSize(), GL_UNSIGNED_INT, NULL);
	glDisable(GL_POLYGON_OFFSET_FILL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// - - - - - - - - - -
	glLineWidth(1.0f);

	glEnable(GL_LIGHTING);
	glPopMatrix();

}

uint C_Mesh::GetVerticesSize() const
{
	return rMesh ? rMesh->GetVerticesSize() : 0;
}

std::vector<float3>* C_Mesh::GetVertices()
{
	return rMesh ? rMesh->GetVertices() : nullptr;
}

float3* C_Mesh::GetVerticesIndex()
{
	return rMesh ? rMesh->GetVerticesIndex() : nullptr;
}

std::vector<uint>* C_Mesh::GetIndices()
{
	return rMesh ? rMesh->GetIndices() : nullptr;
}

uint C_Mesh::GetIndicesSize() const
{
	return rMesh ? rMesh->GetIndicesSize() : 0;
}

uint* C_Mesh::GetIndicesIndex()
{
	return rMesh ? rMesh->GetIndicesIndex() : nullptr;
}

std::vector<float3>* C_Mesh::GetNormals()
{
	return rMesh ? rMesh->GetNormals() : nullptr;
}

uint C_Mesh::GetNormalsSize() const
{
	return rMesh ? rMesh->GetNormalsSize() : 0;
}

float3* C_Mesh::GetNormalsIndex()
{
	return rMesh ? rMesh->GetNormalsIndex() : nullptr;
}

std::vector<float2>* C_Mesh::GetTextCoords()
{
	return rMesh ? rMesh->GetTextCoords() : nullptr;
}

uint C_Mesh::GetTextureCoordsSize() const
{
	return rMesh ? rMesh->GetTextureCoordsSize() : 0;
}

float2* C_Mesh::GetTextureCoordsIndex()
{
	return rMesh ? rMesh->GetTextureCoordsIndex() : nullptr;
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

void C_Mesh::SetResourceId(uint newId)
{
	App->resources->UnloadResource(rMeshId);

	rMeshId = newId;
	rMesh = (R_Mesh*)App->resources->RequestResource(rMeshId);
}

int C_Mesh::GetResourceId()
{
	return rMeshId;
}

std::string C_Mesh::GetName() const
{
	return rMesh ? rMesh->GetName() : "No Mesh Loaded";
}

std::string C_Mesh::GetPath() const
{
	return rMesh ? rMesh->GetLibraryPath() : "No Mesh Loaded";
}

