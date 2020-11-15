#include "Application.h"
#include "MeshLoader.h"
#include "M_FileManager.h"
#include "M_Scene.h"

#include "GameObject.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Material.h"

#include "TextureLoader.h"
#include "MaterialLoader.h"

#include "p2Defs.h"

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

void MeshLoader::Load(const char* path)
{
	char* buffer = nullptr;

	uint bytesFile = App->physFS->Load(path, &buffer);

	if (bytesFile == 0)
	{
		return;
	}

	const aiScene* scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	RELEASE_ARRAY(buffer);

	if (scene)
	{
		//WARNING: assuming that all the mesh is made from triangles
		std::string fbxName = "";
		App->physFS->SplitFilePath(path, nullptr, nullptr, &fbxName, nullptr);
		GameObject* sceneRoot = App->scene->CreateGameObject(fbxName.c_str(), nullptr);

		Private::LoadAiSceneMeshes(scene, scene->mRootNode, path, sceneRoot);

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading aiScene %s", path);
	}

}

void MeshLoader::Import(const char* path)
{
	char* buffer = nullptr;

	uint bytesFile = App->physFS->Load(path, &buffer);

	if (bytesFile == 0)
	{
		return;
	}

	const aiScene* scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	RELEASE_ARRAY(buffer);

	if (scene)
	{
		LOG("FileÑ: %s Succesfully Imported!! Trying to convert to own format. . .", path);
		Save(scene, path);
	}
	else
	{
		LOG("Error importing aiScene %s", path);
	}
}

void MeshLoader::Save(const aiScene* scene, const char* path)
{
	std::string fbxName = "";
	App->physFS->SplitFilePath(path, nullptr, nullptr, &fbxName, nullptr);

	GameObject* sceneRoot = App->scene->CreateGameObject(fbxName.c_str(), nullptr);

	Private::LoadAiSceneMeshes(scene, scene->mRootNode, path, sceneRoot);

	aiReleaseImport(scene);
}

bool MeshLoader::Private::LoadAiSceneMeshes(const aiScene* scene, const aiNode* parent, const char* filePath, GameObject* gbParent, float4x4 transform)
{
	for (int i = 0; i < parent->mNumChildren; i++)
	{
		LoadNodeMeshes(scene, parent->mChildren[i], filePath, gbParent, transform);
	}

	return true;
}

bool MeshLoader::Private::LoadNodeMeshes(const aiScene* scene, const aiNode* node, const char* filePath, GameObject* parent, float4x4 transform)
{
	GameObject* newGameObject = nullptr;

	aiVector3D translation, scaling;
	aiQuaternion rotation;
	node->mTransformation.Decompose(scaling, rotation, translation);
	float3 pos(translation.x, translation.y, translation.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);
	float3 scale(scaling.x, scaling.y, scaling.z);

	if (node->mNumMeshes > 0)
	{
		newGameObject = App->scene->CreateGameObject(node->mName.C_Str(), parent);

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

			//TODO: This should be a Mesh Resource
			//MESH IMPORT & SAVE ----------------------------------------
			C_Mesh* newMesh = new C_Mesh();

			std::string path = "";
			std::string meshName = "";
			std::string meshExtension = "";
			App->physFS->SplitFilePath(filePath, nullptr, &path, &meshName, &meshExtension);
			newMesh->GenerateMesh(std::string(meshName + "." + meshExtension).c_str(), filePath, vertices, indices, normals, textureCoords);

			SaveMesh(newMesh, std::string(LIBRARY_MESHES + meshName + "_" + node->mName.C_Str() + ".mesh").c_str());
			RELEASE(newMesh);

			//MATERIAL IMPORT & SAVE----------------------------------------
			if (scene->HasMaterials())
			{
				C_Material* newMaterial = new C_Material();

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				MaterialLoader::IImport(material, newMaterial, filePath);
				std::string savedPath = MaterialLoader::Save(newMaterial);
				RELEASE(newMaterial);
				C_Material* loadedmaterial = new C_Material();
				MaterialLoader::Load(loadedmaterial, savedPath.c_str());
				newGameObject->AddComponent(loadedmaterial);

			}

			//LOAD--------------------
			C_Mesh* myNewMesh = new C_Mesh();
			LoadMesh(myNewMesh, std::string(LIBRARY_MESHES + meshName + "_" + node->mName.C_Str() + ".mesh").c_str());
			newGameObject->AddComponent(myNewMesh);

			//Transform Load------
			newGameObject->GetComponent<C_Transform>()->SetLocalTransformation(pos, rot, scale);
			parent->GetComponent<C_Transform>()->SetLocalTransformation(transform);
			newGameObject->GetComponent<C_Transform>()->GenerateGlobalTransformationFrom(parent->GetComponent<C_Transform>()->GetGlobalTransform());
		}
	}

	//We calculate the transform & send it to the recursive interaction
	// Because we avoid the "empty" TRS parents that assimp generates but we want to keep the Transformation
	transform = transform * float4x4::FromTRS(pos, rot, scale);

	LoadAiSceneMeshes(scene, node, filePath, newGameObject == nullptr ? parent : newGameObject, transform);

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

void MeshLoader::Private::SaveMesh(C_Mesh* mesh, const char* path)
{
	// amount of indices / vertices /  normals / texture_coords / AABB
	uint ranges[4] = { mesh->GetIndicesSize(), mesh->GetVerticesSize(), mesh->GetNormalsSize(), mesh->GetTextureCoordsSize() };
	uint size = sizeof(ranges) + sizeof(uint) * mesh->GetIndicesSize() + sizeof(float3) * mesh->GetVerticesSize()
		+ sizeof(float3) * mesh->GetNormalsSize() + sizeof(float2) * mesh->GetNormalsSize();

	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;
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

	App->physFS->Save(path, fileBuffer, size);

	RELEASE_ARRAY(fileBuffer);
}

void MeshLoader::Private::LoadMesh(C_Mesh* myNewMesh, const char* path)
{
	char* buffer = nullptr;
	App->physFS->Load(path, &buffer);

	if (buffer != nullptr)
	{
		char* cursor = buffer;
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

		//TODO: When the resources are implemented, this has to fly away¬¬
		std::string meshName = "";
		App->physFS->SplitFilePath(path, nullptr, nullptr, &meshName, nullptr);

		myNewMesh->SetAABB();
		myNewMesh->GenerateMesh(meshName.c_str(), path);

		RELEASE_ARRAY(buffer);
	}
	else
	{
		LOG("Coudn't load mesh from internal library...");
	}

}

