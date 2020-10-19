#include "Application.h"
#include "MeshLoader.h"
#include "Mesh.h"
#include "M_FileManager.h"


#include "p2Defs.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float2.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"


#include "Assimp/include/mesh.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

void MeshLoader::Init()
{
	if (_DEBUG == 1)
	{
		// Stream log messages to Debug window
		struct aiLogStream stream;
		stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
		aiAttachLogStream(&stream);
	}
}

void MeshLoader::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();
}

std::vector<Mesh> MeshLoader::Load(const char* path)
{
	char* buffer;
	
	uint bytesFile = App->physFS->Load(path, &buffer);
	const aiScene* scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);


	std::vector<Mesh> loadedMeshes;
	bool ret = true;

	if (scene)
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		//WARNING: assuming that all the mesh is made from triangles
		ret = LoadSceneMeshes(scene, loadedMeshes);

		if (ret && loadedMeshes.size() > 0)
		{
			LOG("Loaded %i mesh(es)!", loadedMeshes.size())
		}
		aiReleaseImport(scene);
	}

	if (!scene || !ret)
	{
		LOG("Error loading scene %s", path);
	}


	return loadedMeshes;
}

bool MeshLoader::LoadSceneMeshes(const aiScene* scene, std::vector<Mesh>& loadedMeshes)
{
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];

		std::vector<Vertex> vertices;
		std::vector<uint> indices;

		vertices.reserve(mesh->mNumVertices);
		LoadVertices(mesh, vertices);
		if (vertices.size() == 0)
		{
			LOG("Error loading vertices in scene")
				return false;
		}
		else
			LOG("New mesh with %i vertices", vertices.size());

		indices.reserve(mesh->mNumFaces * 3);
		bool ret = LoadIndices(mesh, indices);
		if (indices.size() == 0 || !ret)
		{
			LOG("Error loading indices in scene")
				return false;
		}
		else
			LOG("New mesh with %i indices", indices.size());


		loadedMeshes.push_back(Mesh(vertices, indices));
	}

	return true;
}

void MeshLoader::LoadVertices(aiMesh* mesh, std::vector<Vertex>& vertices)
{
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		float3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			float2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = float2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
}

bool MeshLoader::LoadIndices(aiMesh* mesh, std::vector<uint>& indices)
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

