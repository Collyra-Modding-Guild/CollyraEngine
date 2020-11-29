#ifndef __ModelLoader_H__
#define __ModelLoader_H__

#include <vector>
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"

class JsonConfig;
class JsonArray;
class R_Model;
struct ModelNode;
class aiScene;
class aiNode;

namespace ModelLoader
{

	void ImportModel(const char* path, char** buffer, unsigned int bufferSize, R_Model* resourceModel);
	uint Save(const R_Model* rModel, char** buffer);
	void Load(R_Model* rModel, char* buffer);

	namespace Private
	{
		bool LoadAiSceneMeshes(const aiScene* scene, const aiNode* root, const char* filePath, R_Model* myResource, unsigned int ID = INT_MAX, float4x4 transform = float4x4::identity);
		void LinkModelResources(R_Model* model, const std::vector<uint>& meshes, const std::vector<uint>& materials);
		bool LoadNodeMeshes(const aiScene* scene, const aiNode* node, const char* filePath, R_Model* myResource, unsigned int panretId = INT_MAX, float4x4 transform = float4x4::identity);
		void SaveGameModel(const ModelNode* gameObject, JsonConfig saveTo, JsonArray* gameObjectsArray);
	}

}


#endif //__ModelLoader_H__