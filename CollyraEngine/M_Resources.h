#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"
#include <vector>
#include <map>
#include <string>
#include "p2Defs.h"
#include "Globals.h"
#include "Timer.h"

#include "MathGeoLib/include/Algorithm/Random/LCG.h"

class Mesh;
class Resource;
enum class R_TYPE;
class R_Model;
struct PathNode;

class M_Resources : public Module
{
public:

	M_Resources(MODULE_TYPE type, bool startEnabled = true);
	~M_Resources();

	bool Awake();
	bool Start();
	updateStatus Update(float dt) override;
	bool CleanUp();

	//Resource------------
	//Import
	uint ImportResourceFromAssets(const char* path);
	 
	//Load & Request
	Resource* RequestResource(uint id);
	Resource* LoadResource(uint id, const char* assetsPath = nullptr);

	//Creation
	Resource* CreateResource(R_TYPE rType, uint32 forceId = 0);

	//Save
	bool SaveResource(Resource* toSave, std::string assetsPath, bool saveMeta = true);
	bool SaveMeta(Resource* toSave, std::string assetsPath);

	//Destruction
	bool DeleteResource(uint32 idToDestroy);
	void UnloadResource(Resource* toUnload);

	//Helpers
	R_TYPE GetResourceTypeFromExtension(const char* rPath);
	bool GetInfoFromMeta(const char* metaPath, uint32* id = nullptr, uint32* modDate = nullptr, R_TYPE* type = nullptr, std::string* path = nullptr);
	uint32 GenerateId();

private:
	void ImportModel(const char* path, char** buffer, unsigned int bufferSize, R_Model* resourceModel);
	void ImportAllAssets();
	void CheckAssetsImport(PathNode assetsFiles);
	std::string DuplicateFile(const char* path);

	uint32 ImportResource(std::string path, uint32 forceid = 0);

public: 
	int defaultTextureId;

private:
	std::map<uint, Resource*> resourceMap;
	LCG randomGen;
	Timer updateAssetsTimer;

};

#endif // __ModuleResources_H__
