#include "MeshLoader.h"
#include "Mesh.h"

#include "p2Defs.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float2.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
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
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	std::vector<Mesh> loadedMeshes;

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			std::vector<Vertex> vertices;
			std::vector<uint> indices;

			aiMesh* mesh = scene->mMeshes[i];

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
			LOG("New mesh with %i vertices", vertices.size());


			for (uint i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);

				for (uint j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			LOG("New mesh with %i indices", indices.size());

			loadedMeshes.push_back(Mesh(vertices, indices));
		}
		LOG("Loaded %i mesh(es)!", loadedMeshes.size())
			aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene % s", path);


	return loadedMeshes;
}
