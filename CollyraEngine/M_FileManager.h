#ifndef __ModuleFileManager_H__
#define __ModuleFileManager_H__

#include "Module.h"

#include <vector>
#include <string>

class JsonConfig;
struct PathNode;

#define ASSETS_FOLDER "/Assets"
#define ASSETS_MESHES_PATH "/Assets/Meshes"
#define ASSETS_TEXTYRES_PATH "/Assets/Textures"

#define LIBRARY_PATH "/Library"
#define LIBRARY_MESHES "/Library/Meshes/"
#define LIBRARY_TEXTURES "/Library/Textures/"
#define LIBRARY_SCENES "/Library/Scenes/"
#define LIBRARY_MODELS "/Library/Models/"
#define LIBRARY_MATERIALS "/Library/Materials/"


class M_FileManager : public Module
{
public:

	M_FileManager(MODULE_TYPE type, bool startEnabled = true);
	~M_FileManager();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp() override;

	void CreateFolderDirs();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) const;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) const;

	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const;
	
	//Extension Utils-----------
	bool HasExtension(const char* path) const;
	bool HasExtension(const char* path, std::string extension) const;
	bool HasExtension(const char* path, std::vector<std::string> extensions) const;

	//Path Manipulations Utils--------------
	std::string NormalizePath(const char* path) const;
	std::string LowerCaseString(const char* path) const;
	//Gets a path to a file in the project folder & a relative path from that file & returns the combination relative to the project
	void SplitFilePath(const char* full_path, std::string* projectPath, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;
	std::string GetProjectPathFromInternalRelative(std::string& projectPath, std::string& pathInternalRelative) const;

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;

	bool ImportFile(const char* file, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);

	unsigned int	Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	bool			Remove(const char* file);


	// GET - - -
	void			GetRealDir(const char* path, std::string& output) const;
	void			GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const;
	const char*		GetWriteDir() const;
	uint64			GetLastModTime(const char* filename);
	std::string		GetUniqueName(const char* path, const char* name) const;
	std::string		GetExtensionFolder(const char* fileExtension);
	std::string		GetPathRelativeToAssets(const char* originalPath) const;
	PathNode		GetAllFiles(const char* directory, std::vector<std::string>* filter_ext = nullptr, std::vector<std::string>* ignore_ext = nullptr) const;

private:
	int CheckPath(const char*);

private:
	std::vector<std::string>	pathVector;
};

#endif // __ModuleFileManager_H__
