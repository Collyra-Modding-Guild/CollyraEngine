#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"
#include <vector>
#include <map>
#include <string>
#include "p2Defs.h"

#include "MathGeoLib/include/Algorithm/Random/LCG.h"

class Mesh;
class Resource;
enum class R_TYPE;
class R_Model;

class M_Resources : public Module
{
public:

	M_Resources(MODULE_TYPE type, bool startEnabled = true);
	~M_Resources();

	bool Awake();
	bool Start();

	bool CleanUp();

	//Resource------------
	//Import
	uint ImportResource(const char* path);

	//Load & Request
	Resource* RequestResource(uint id);
	Resource* LoadResource(const char* path, uint id);

	//Creation
	Resource* CreateResource(R_TYPE rType);

	//Save
	bool SaveResource(Resource* toSave, std::string assetsPath, bool saveMeta = true);
	bool SaveMeta(Resource* toSave, std::string assetsPath);

	//Destruction
	bool UnloadResource(uint32 idToDestroy);

	//Helpers
	R_TYPE GetResourceTypeFromExtension(const char* rPath);
	uint32 GenerateId();

private:
	void ImportModel(const char* path, char** buffer, unsigned int bufferSize, R_Model* resourceModel);
	std::string DuplicateFile(const char* path);

public: 
	int defaultTextureId;

private:
	std::map<uint, Resource*> resourceMap;
	LCG randomGen;
};

#endif // __ModuleResources_H__
