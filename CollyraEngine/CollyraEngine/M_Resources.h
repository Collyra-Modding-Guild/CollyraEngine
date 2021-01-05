#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"
#include <vector>
#include <map>
#include <string>
#include "p2Defs.h"
#include "Globals.h"
#include "Timer.h"
#include "PathNode.h"

#include "MathGeoLib/include/Algorithm/Random/LCG.h"

class Mesh;
class Resource;
enum class R_TYPE;
class R_Model;

enum class ASSETS_CHECK
{
	TO_CHECK,
	TO_IMPORT,
	TO_CHANGE
};

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
	Resource* CreateResource(R_TYPE rType, std::string name = "", uint32 forceId = 0);

	//Save
	uint SaveResource(Resource* toSave, std::string assetsPath, bool saveMeta = true);

	//Destruction
	bool DeleteResource(uint32 idToDestroy);
	void UnloadResource(Resource* toUnload);
	void UnloadResource(uint32 toUnloadId);

	//Helpers
	R_TYPE GetResourceTypeFromExtension(const char* rPath);
	bool GetInfoFromMeta(const char* metaPath, uint32* id = nullptr, uint32* modDate = nullptr, R_TYPE* type = nullptr, std::string* path = nullptr);
	uint32 GenerateId();
	std::string FindLibraryFile(uint id);
	void SetDeleteResources(bool newState);
	void GetResourcesByType(std::vector<const Resource*>& resources, R_TYPE type) const;
	PathNode* GetAllAssetFiles();
	PathNode GetAllLibraryFiles();
	void UpdateChangedResources();
	uint CheckAssetInMeta (std::string metaPath, std::string relativePath);

	uint LoadDefaultScene();

	void NotifyHotReload();

private:
	void SearchAllAssetFiles();
	void CheckAssetsImport(PathNode& pathnode);
	std::string CheckMetaFilesId(PathNode& metasOnly, uint searchFor);
	std::string CheckAssetFolderForId(uint searchFor);
	std::string DuplicateFile(const char* path);
	std::string GetDefaultResourceName(R_TYPE type) const;

	uint32 ImportResource(std::string path, uint32 forceid = 0);
	void CheckResourcesToUnload();

	//Save
	bool SaveMeta(Resource* toSave, std::string assetsPath);

public:
	int defaultTextureId;

private:
	std::map<uint, Resource*> resourceMap;
	LCG randomGen;
	Timer updateAssetsTimer;
	bool deleteResources;

	std::map<uint, bool> onlineIdUpdated;
	PathNode allLibFiles;
	PathNode allAssetFiles;
	ASSETS_CHECK assetsRead;

};

#endif // __ModuleResources_H__
