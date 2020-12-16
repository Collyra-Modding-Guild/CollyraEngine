#ifndef __MeshLoader_H__
#define __MeshLoader_H__

#include <vector>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class aiMesh;
class aiScene;
class R_Mesh;
class C_Mesh;
class aiNode;
class GameObject;
class aiMaterial;
class R_Model;
class aiMesh;

namespace MeshLoader
{
	void Init();
	void CleanUp();

	uint ImportMeshFromModel(const char* path, aiMesh* myMesh);
	uint Save(R_Mesh* mesh, char** buffer);

	namespace Private
	{
		bool LoadAiSceneMeshes(const aiScene* scene, const aiNode* root, const char* filePath, R_Model* myResource, unsigned int ID = INT_MAX, float4x4 transform = float4x4::identity);
		bool LoadNodeMeshes(const aiScene* scene, const aiNode* node, const char* filePath, R_Model* myResource, unsigned int panretId = INT_MAX, float4x4 transform = float4x4::identity);
		void LoadVertices(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords);
		bool LoadIndices(aiMesh* mesh, std::vector<uint>& indices);


		void LoadMesh(R_Mesh* myNewMesh, char** buffer, uint id);
	}
}


#endif //__MeshLoader_H__