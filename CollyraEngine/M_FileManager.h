#ifndef __ModuleFileManager_H__
#define __ModuleFileManager_H__

#include "Module.h"

#include "PhysFS/include/physfs.h"
#include "SDL/include/SDL.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

#include <vector>
#include <string>

class M_FileManager : public Module
{
public:

	M_FileManager(MODULE_TYPE type, bool start_enabled = true);
	~M_FileManager();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

	// Return the bytes of a PhysFS filehandle
	uint Load(const char* path, char** buffer) const;

	// Allows you to use pointers to memory instead of files or things such as images or samples
	SDL_RWops* Load(const char* path);

	bool M_FileManager::HasExtension(const char* path) const;
	bool M_FileManager::HasExtension(const char* path, std::string extension) const;
	bool M_FileManager::HasExtension(const char* path, std::vector<std::string> extensions) const;

	void M_FileManager::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) const;

private:
	int CheckPath(const char*);

private:
	std::vector<std::string> pathVector;
	std::vector<char*> bufferVector;
	std::vector<uint> bytesVector;
};

#endif // __ModuleFileManager_H__
