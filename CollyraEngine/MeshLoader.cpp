#include "Application.h"
#include "MeshLoader.h"
#include "M_FileManager.h"
#include "M_Resources.h"

#include "R_Model.h"
#include "R_Material.h"
#include "R_Mesh.h"
#include "C_Mesh.h"

#include "TextureLoader.h"
#include "MaterialLoader.h"

#include "p2Defs.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#include "Assimp/include/mesh.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

void MeshLoader::Init()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void MeshLoader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();
}


uint MeshLoader::ImportMeshFromModel(const char* filePath, aiMesh* myMesh)
{
	uint MeshID = 0;

	aiMesh* mesh = myMesh;

	std::vector<float3> vertices;
	std::vector<float3> normals;
	std::vector<float2> textureCoords;
	std::vector<uint> indices;

	vertices.reserve(mesh->mNumVertices);
	textureCoords.reserve(mesh->mNumVertices);
	if (mesh->HasNormals())
		normals.reserve(mesh->mNumVertices);

	Private::LoadVertices(mesh, vertices, normals, textureCoords);
	if (vertices.size() == 0)
	{
		LOG("Error loading vertices in scene")
			return 0;
	}

	indices.reserve(mesh->mNumFaces * 3);
	bool ret = Private::LoadIndices(mesh, indices);
	if (indices.size() == 0 || !ret)
	{
		LOG("Error loading indices in scene")
			return 0;
	}
	LOG("New mesh with %i vertices & %i indices", vertices.size(), indices.size());

	//MESH IMPORT & SAVE ----------------------------------------
	std::string path = "";
	std::string meshName = "";
	std::string meshExtension = "";
	App->physFS->SplitFilePath(filePath, nullptr, &path, &meshName, &meshExtension);

	R_Mesh* newMesh = (R_Mesh*)App->resources->CreateResource(R_TYPE::MESH);
	MeshID = newMesh->GetUid();
	newMesh->GenerateMesh(vertices, indices, normals, textureCoords);

	App->resources->SaveResource(newMesh, filePath, false);
	App->resources->DeleteResource(MeshID);

	return MeshID;
}

uint MeshLoader::Save(R_Mesh* mesh, char** fileBuffer)
{
	// amount of indices / vertices /  normals / texture_coords / AABB
	uint ranges[4] = { mesh->GetIndicesSize(), mesh->GetVerticesSize(), mesh->GetNormalsSize(), mesh->GetTextureCoordsSize() };
	uint size = sizeof(ranges) + sizeof(uint) * mesh->GetIndicesSize() + sizeof(float3) * mesh->GetVerticesSize()
		+ sizeof(float3) * mesh->GetNormalsSize() + sizeof(float2) * mesh->GetNormalsSize();

	*fileBuffer = new char[size]; // Allocate
	char* cursor = *fileBuffer;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(uint) * mesh->GetIndicesSize();
	memcpy(cursor, mesh->GetIndicesIndex(), bytes);
	cursor += bytes;

	// Store vertices
	bytes = sizeof(float3) * mesh->GetVerticesSize();
	memcpy(cursor, mesh->GetVerticesIndex(), bytes);
	cursor += bytes;

	// Store normals
	bytes = sizeof(float3) * mesh->GetNormalsSize();
	memcpy(cursor, mesh->GetNormalsIndex(), bytes);
	cursor += bytes;

	// Store textureCoords
	bytes = sizeof(float2) * mesh->GetTextureCoordsSize();
	memcpy(cursor, mesh->GetTextureCoordsIndex(), bytes);
	cursor += bytes;

	return size;
}


void MeshLoader::Private::LoadVertices(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords)
{
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		// process vertex positions, normals and texture coordinates
		float3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		vertices.push_back(vector);

		if (mesh->HasNormals())
		{
			float3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			normals.push_back(normal);
		}

		float2 textCoord;
		if (mesh->mTextureCoords[0])
		{
			textCoord.x = mesh->mTextureCoords[0][i].x;
			textCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
			textCoord = float2(0.0f, 0.0f);

		textureCoords.push_back(textCoord);
	}
}

bool MeshLoader::Private::LoadIndices(aiMesh* mesh, std::vector<uint>& indices)
{
	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices != 3)
		{
			LOG("ERROR loading Mesh! At least 1 face is not made of triangles!")
				return false;
		}

		for (uint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return true;
}



void MeshLoader::Private::LoadMesh(R_Mesh* myNewMesh, char** buffer, uint id)
{
	char* cursor = *buffer;
	// amount of indices / vertices / normals / texture_coords / AABB
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	// Load indices
	bytes = sizeof(uint) * ranges[0];
	myNewMesh->GetIndices()->resize(ranges[0]);
	memcpy(myNewMesh->GetIndicesIndex(), cursor, bytes);
	cursor += bytes;

	// Load vertices
	bytes = sizeof(float3) * ranges[1];
	myNewMesh->GetVertices()->resize(ranges[1]);
	memcpy(myNewMesh->GetVerticesIndex(), cursor, bytes);
	cursor += bytes;

	// Load Normals
	bytes = sizeof(float3) * ranges[2];
	myNewMesh->GetNormals()->resize(ranges[2]);
	memcpy(myNewMesh->GetNormalsIndex(), cursor, bytes);
	cursor += bytes;

	// Load Texture Coords
	bytes = sizeof(float2) * ranges[3];
	myNewMesh->GetTextCoords()->resize(ranges[3]);
	memcpy(myNewMesh->GetTextureCoordsIndex(), cursor, bytes);
	cursor += bytes;


	myNewMesh->SetAABB();
	myNewMesh->GenerateMesh();
}

