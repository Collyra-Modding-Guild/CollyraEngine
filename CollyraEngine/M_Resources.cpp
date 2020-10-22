#include "M_Resources.h"

#include "MeshLoader.h"
#include "TextureLoader.h"

M_Resources::M_Resources(MODULE_TYPE type, bool start_enabled) : Module(type, start_enabled)
{}

M_Resources::~M_Resources()
{}

bool M_Resources::Awake()
{
	//Loaders Initialization---------------------
	MeshLoader::Init();
	TextureLoader::Init();

	return true;
}

bool M_Resources::CleanUp()
{
	//Loaders CleanUp---------------------
	MeshLoader::CleanUp();
	TextureLoader::CleanUp();

	return true;
}
