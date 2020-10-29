#include "Application.h"
#include "MeshLoader.h"
#include "M_FileManager.h"
#include "M_Scene.h"

#include "GameObject.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Material.h"

#include "TextureLoader.h"

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

		LoadSceneMeshes(scene, scene->mRootNode, path, sceneRoot);

		aiReleaseImport(scene);
	}

	if (!scene)
	{
		LOG("Error loading scene %s", path);
	}

}

bool MeshLoader::LoadSceneMeshes(const aiScene* scene, const aiNode* parent, const char* filePath, GameObject* gbParent, float4x4 transform)
{
	for (int i = 0; i < parent->mNumChildren; i++)
	{
		LoadNodeMeshes(scene, parent->mChildren[i], filePath, gbParent, transform);
	}

	return true;
}

bool MeshLoader::LoadNodeMeshes(const aiScene* scene, const aiNode* node, const char* filePath, GameObject* parent, float4x4 transform)
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
		GameObject* newGameObject = App->scene->CreateGameObject(node->mName.C_Str(), parent);

		//Transform Load------

		newGameObject->GetComponent<C_Transform>()->SetLocalTransformation(pos, rot, scale);
		parent->GetComponent<C_Transform>()->SetLocalTransformation(transform);
		newGameObject->GetComponent<C_Transform>()->GenerateGlobalTransformationFrom(transform);

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
			if (mesh->HasNormals())
				normals.reserve(mesh->mNumVertices);
			textureCoords.reserve(mesh->mNumVertices);

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

			C_Mesh* newMesh = (C_Mesh*)newGameObject->CreateComponent(COMPONENT_TYPE::MESH);

			std::string path = "";
			std::string meshName = "";
			std::string meshExtension = "";
			App->physFS->SplitFilePath(filePath, nullptr, &path, &meshName, &meshExtension);

			newMesh->GenerateMesh(std::string(meshName + "." + meshExtension).c_str(), filePath, vertices, indices, normals, textureCoords);

			//Materials Load------------
			if (scene->HasMaterials())
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				if (material != nullptr)
					LoadMaterialFromMesh(material, newGameObject, filePath);
			}

		}

	}

	transform = transform * float4x4::FromTRS(pos, rot, scale);

	if (newGameObject == nullptr)
	{
		LoadSceneMeshes(scene, node, filePath, parent, transform);

	}
	else
	{
		LoadSceneMeshes(scene, node, filePath, newGameObject, transform);
	}


	return true;
}

void MeshLoader::LoadMaterialFromMesh(const aiMaterial* mat, GameObject* newGameObject, const char* meshPath)
{
	aiString path;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	C_Material* newMaterial = nullptr;
	aiColor4D materialColor;
	if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &materialColor) == aiReturn_SUCCESS)
	{
		newMaterial = (C_Material*)newGameObject->CreateComponent(COMPONENT_TYPE::MATERIAL);
		newMaterial->SetColor(Color{ materialColor.r, materialColor.g, materialColor.b, materialColor.a });
	}

	if (path.length > 0)
	{
		std::string loadedPath = "";

		std::string pathRelativeToFbx = path.C_Str();
		std::string relativePath = "";
		App->physFS->SplitFilePath(meshPath, nullptr, &relativePath, nullptr, nullptr);
		pathRelativeToFbx = App->physFS->GetProjectPathFromInternalRelative(relativePath, pathRelativeToFbx);
		loadedPath = pathRelativeToFbx;

		uint loadTexture = TextureLoader::Load(pathRelativeToFbx.c_str());
		if (loadTexture == 0)
		{
			LOG("Checking if texture exists in Textures Folder....");
			std::string textureName = "";
			std::string textureExtension = "";
			App->physFS->SplitFilePath(path.C_Str(), nullptr, nullptr, &textureName, &textureExtension);
			std::string defaultPath = TEXTYRES_PATH;
			defaultPath += "/" + textureName + "." + textureExtension;
			loadTexture = TextureLoader::Load(defaultPath.c_str());
			loadedPath = defaultPath;
		}

		if (loadTexture != 0)
		{
			LOG("Texture loaded! ID: %i", loadTexture);

			if (newMaterial == nullptr)
				newMaterial = (C_Material*)newGameObject->CreateComponent(COMPONENT_TYPE::MATERIAL);

			std::string materialName = "";
			std::string materialExtension = "";
			App->physFS->SplitFilePath(loadedPath.c_str(), nullptr, nullptr, &materialName, &materialExtension);

			newMaterial->SetTexture(loadTexture);
			newMaterial->SetTextureNameAndPath(std::string(materialName + "." + materialExtension).c_str(), loadedPath.c_str());


		}
		else
		{
			LOG("Material found but could not be loaded. Texture not found in path given by scene: %s", pathRelativeToFbx.c_str());
		}
	}
}


void MeshLoader::LoadVertices(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords)
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

