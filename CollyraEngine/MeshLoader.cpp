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


void MeshLoader::Import(const char* path)
{
	//char* buffer = nullptr;

	//uint bytesFile = App->physFS->Load(path, &buffer);

	//if (bytesFile == 0)
	//{
	//	return;
	//}

	//const aiScene* scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	//RELEASE_ARRAY(buffer);

	//if (scene)
	//{
	//	LOG("FileÑ: %s Succesfully Imported!! Trying to convert to own format. . .", path);
	//	Save(scene, path);
	//}
	//else
	//{
	//	LOG("Error importing aiScene %s", path);
	//}
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


bool MeshLoader::Private::LoadAiSceneMeshes(const aiScene* scene, const aiNode* parent, const char* filePath, R_Model* myResource, unsigned int parentID, float4x4 transform)
{
	for (int i = 0; i < parent->mNumChildren; i++)
	{
		LoadNodeMeshes(scene, parent->mChildren[i], filePath, myResource, parentID, transform);
	}

	return true;
}

bool MeshLoader::Private::LoadNodeMeshes(const aiScene* scene, const aiNode* node, const char* filePath, R_Model* myResource, unsigned int panretId, float4x4 transform)
{
	//GameObject* newGameObject = nullptr;
	unsigned int myID = panretId;

	aiVector3D translation, scaling;
	aiQuaternion rotation;
	node->mTransformation.Decompose(scaling, rotation, translation);
	float3 pos(translation.x, translation.y, translation.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	float3 scale(scaling.x, scaling.y, scaling.z);

	transform = transform * float4x4::FromTRS(pos, rot, scale);

	if (node->mNumMeshes > 0)
	{
		//newGameObject = App->scene->CreateGameObject(node->mName.C_Str(), parent);
		uint32 meshId = 0;
		uint32 materialId = 0;

		//Mesh Load---------
		int meshSize = node->mNumMeshes;
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			std::vector<float3> vertices;
			std::vector<float3> normals;
			std::vector<float2> textureCoords;
			std::vector<uint> indices;

			vertices.reserve(mesh->mNumVertices);
			textureCoords.reserve(mesh->mNumVertices);
			if (mesh->HasNormals())
				normals.reserve(mesh->mNumVertices);

			LoadVertices(mesh, vertices, normals, textureCoords);
			if (vertices.size() == 0)
			{
				LOG("Error loading vertices in scene")
					continue;
			}

			indices.reserve(mesh->mNumFaces * 3);
			bool ret = LoadIndices(mesh, indices);
			if (indices.size() == 0 || !ret)
			{
				LOG("Error loading indices in scene")
					continue;
			}
			LOG("New mesh with %i vertices & %i indices", vertices.size(), indices.size());

			//MESH IMPORT & SAVE ----------------------------------------
			std::string path = "";
			std::string meshName = "";
			std::string meshExtension = "";
			App->physFS->SplitFilePath(filePath, nullptr, &path, &meshName, &meshExtension);

			R_Mesh* newMesh = (R_Mesh*)App->resources->CreateResource(R_TYPE::MESH);
			meshId = newMesh->GetUid();
			newMesh->GenerateMesh(std::string(meshName + "." + meshExtension).c_str(), filePath, vertices, indices, normals, textureCoords);

			App->resources->SaveResource(newMesh, filePath, false);
			App->resources->DeleteResource(meshId);

			//MATERIAL IMPORT & SAVE----------------------------------------
			if (scene->HasMaterials())
			{
				R_Material* newMaterial = (R_Material*)App->resources->CreateResource(R_TYPE::MATERIAL);
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				materialId = newMaterial->GetUid();

				//IMPORT & SAVE
				MaterialLoader::Import(material, newMaterial, node->mName.C_Str(), filePath);
				App->resources->SaveResource(newMaterial, filePath, false);
				App->resources->DeleteResource(materialId);
			}

			//LOAD--------------------
			//Transform Load------
			//newGameObject->GetComponent<C_Transform>()->SetLocalTransformation(pos, rot, scale);
			//parent->GetComponent<C_Transform>()->SetLocalTransformation(transform);
			//newGameObject->GetComponent<C_Transform>()->GenerateGlobalTransformationFrom(parent->GetComponent<C_Transform>()->GetGlobalTransform());

			myID = myResource->AddModelNode(App->resources->GenerateId(), node->mName.C_Str(), transform, panretId, meshId, materialId);
			transform = float4x4::identity;
		}
	}

	LoadAiSceneMeshes(scene, node, filePath, myResource, myID, transform);

	return true;
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
	myNewMesh->GenerateMesh(std::to_string(id).c_str(), "");

}

