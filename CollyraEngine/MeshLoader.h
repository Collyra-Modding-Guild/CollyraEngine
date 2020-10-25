#ifndef __MeshLoader_H__
#define __MeshLoader_H__

#include <vector>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "C_Mesh.h"

class aiMesh;
class aiScene;
class C_Mesh;
class aiNode;
class GameObject;

namespace MeshLoader
{
	void Init();
	void CleanUp();

	void Load(const char* path);

	bool LoadSceneMeshes(const aiScene* scene, const aiNode* root, GameObject* parent = nullptr);
	bool LoadNodeMeshes(const aiScene* scene, const aiNode* node, GameObject* parent = nullptr);
	void LoadVertices(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords);
	bool LoadIndices(aiMesh* mesh, std::vector<uint>& indices);
}


#endif //__MeshLoader_H__