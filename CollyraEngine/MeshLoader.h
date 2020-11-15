#ifndef __MeshLoader_H__
#define __MeshLoader_H__

#include <vector>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class aiMesh;
class aiScene;
class C_Mesh;
class aiNode;
class GameObject;
class aiMaterial;

namespace MeshLoader
{
	void Init();
	void CleanUp();

	void Import(const char* path);
	void Save(const aiScene* scene, const char* path);
	void Load(const char* path);

	namespace Private
	{
		bool LoadAiSceneMeshes(const aiScene* scene, const aiNode* root, const char* filePath, GameObject* parent = nullptr, float4x4 transform = float4x4::identity);
		bool LoadNodeMeshes(const aiScene* scene, const aiNode* node, const char* filePath, GameObject* parent = nullptr, float4x4 transform = float4x4::identity);
		void LoadVertices(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords);
		bool LoadIndices(aiMesh* mesh, std::vector<uint>& indices);

		void SaveMesh(C_Mesh* mesh, const char* path);

		void LoadMesh(C_Mesh* myNewMesh, const char* path);
	}
}


#endif //__MeshLoader_H__