#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"
#include <vector>

class Mesh;

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

	//void CreateMeshesExternal(const char* path);

	void CreateMeshesInternal(const char* path);

public: 

	int defaultTextureId;

};

#endif // __ModuleResources_H__
