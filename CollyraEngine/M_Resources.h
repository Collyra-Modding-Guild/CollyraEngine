#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"
#include <vector>

class Mesh;

class M_Resources : public Module
{
public:

	M_Resources(MODULE_TYPE type, bool start_enabled = true);
	~M_Resources();

	bool Awake();

	bool CleanUp();

	void CreateMeshes(const char* path);

	std::vector<Mesh>* GetMeshes();

public:
	//Meshes----
	std::vector<Mesh> meshes;

private:
	void AddMeshes(std::vector<Mesh>& newMeshes);

};

#endif // __ModuleResources_H__
