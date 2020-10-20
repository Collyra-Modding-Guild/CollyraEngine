#ifndef __MeshLoader_H__
#define __MeshLoader_H__

#include <vector>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Mesh;
class aiMesh;
class aiScene;

namespace MeshLoader
{

	void Init();

	void CleanUp();


	std::vector<Mesh> Load(const char* path);

	bool LoadSceneMeshes(const aiScene* scene, std::vector<Mesh>& loadedMeshes);
	void LoadVertices(aiMesh* mesh, std::vector<float3>& vertices, std::vector<float3>& normals, std::vector<float2>& textureCoords);
	bool LoadIndices(aiMesh* mesh, std::vector<uint>& indices);
}


#endif //__MeshLoader_H__