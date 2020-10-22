#ifndef __ModuleFileManager_H__
#define __ModuleFileManager_H__

#include "Module.h"

#include "PhysFS/include/physfs.h"
#include "SDL/include/SDL.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

#include <vector>
#include <string>

struct SDL_RWops;

struct aiFileIO;

class Config;
struct PathNode;

#define ASSETS_FOLDER "/Assets"
#define MESHES_PATH "/Assets/Meshes"
#define TEXTYRES_PATH "/Assets/Textures"

class M_FileManager : public Module
{
public:

	M_FileManager(MODULE_TYPE type, bool start_enabled = true);
	~M_FileManager();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp() override;

	void CreateFolderDirs();

	// Return the bytes of a PhysFS filehandle
	//uint Load(const char* path, char** buffer) const;

	// Allows you to use pointers to memory instead of files or things such as images or samples
	SDL_RWops* Load(const char* path);

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) const;
	const char* GetWriteDir() const;
	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	void GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const;
	PathNode GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr) const;
	void GetRealDir(const char* path, std::string& output) const;
	std::string GetPathRelativeToAssets(const char* originalPath) const;

	bool HasExtension(const char* path) const;
	bool HasExtension(const char* path, std::string extension) const;
	bool HasExtension(const char* path, std::vector<std::string> extensions) const;

	std::string NormalizePath(const char* path) const;
	std::string LowerCaseString(const char* path) const;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;

	bool ImportFile(const char* file, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);

	std::string GetExtensionFolder(const char* fileExtension);

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool Remove(const char* file);

	uint64 GetLastModTime(const char* filename);
	std::string GetUniqueName(const char* path, const char* name) const;

private:
	int CheckPath(const char*);

private:
	std::vector<std::string> pathVector;
	std::vector<char*> bufferVector;
	std::vector<uint> bytesVector;
};

#endif // __ModuleFileManager_H__
