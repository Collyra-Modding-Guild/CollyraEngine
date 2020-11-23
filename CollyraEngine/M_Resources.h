#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"
#include <vector>
#include <map>
#include <string>

class Mesh;
class Resource;

class M_Resources : public Module
{
public:

	M_Resources(MODULE_TYPE type, bool startEnabled = true);
	~M_Resources();

	bool Awake();
	bool Start();

	bool CleanUp();

	void ImportResourceInternal(const char* path);

private:
	void CreateMeshesInternal(const char* path);
	std::string DuplicateFile(const char* path);

public: 
	int defaultTextureId;

private:
	std::map<uint, Resource*> resourceMap;
};

#endif // __ModuleResources_H__
