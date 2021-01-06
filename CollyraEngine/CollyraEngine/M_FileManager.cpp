#include "Application.h"
#include "M_FileManager.h"
#include "PathNode.h"
#include "R_Resource.h"

#include "PhysFS/include/physfs.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

#include <fstream>
#include <filesystem>

M_FileManager::M_FileManager(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled)
{
	// Initialize the PhysicsFS library
	PHYSFS_init(nullptr);

	// We only need this when compiling in debug. In Release we don't need it.
	PHYSFS_mount(".", nullptr, 1);
	AddPath(SCRIPT_PROJECT);
	
	// Enable PhysFS writting
	if (PHYSFS_setWriteDir(".") == 0)
	{
		LOG("PhysFS error while initializing writting dir: %s\n", PHYSFS_getLastError());
	}

	CreateFolderDirs();
}

M_FileManager::~M_FileManager()
{
	PHYSFS_deinit();
}

bool M_FileManager::Awake()
{
	// Determine if the PhysicsFS library is initialized, we can check it for avoid errors.
	if (PHYSFS_isInit())
	{
		LOG("Asset Manager is succefully loaded");
	}
	else
		LOG("Failed loading Asset Manager");



	return true;
}

bool M_FileManager::CleanUp()
{
	pathVector.clear();

	return false;
}

void M_FileManager::CreateFolderDirs()
{
	// If the standard folders do not exist, create them
	std::vector<const char*> dirs = { ASSETS_FOLDER, ASSETS_TEXTURES_PATH,LIBRARY_PATH,LIBRARY_MESHES,
		LIBRARY_TEXTURES,LIBRARY_SCENES,LIBRARY_MODELS, LIBRARY_MATERIALS, ASSETS_MODELS_PATH,
	LIBRARY_SCRIPTS, ASSETS_SCRIPTS_PATH };

	for (uint i = 0; i < dirs.size(); ++i)
	{
		CreateDir(dirs[i]);
	}
}

uint M_FileManager::Load(const char* path, char** buffer) const
{
	uint ret = 0;

	// The reading offset is set to the first byte of the file.
	// Returns a filehandle on success that we will need for the PHYSFS_fileLength
	PHYSFS_file* file = PHYSFS_openRead(path);

	if (file == nullptr)
	{
		LOG("Error loading path. Check the file is in the project folder: % s", path, "ERROR: % s", PHYSFS_getLastError());
		return 0;
	}

	// Check for end-of-file state on a PhysicsFS filehandle.
	if (!PHYSFS_eof(file))
	{
		// Get total length of a file in bytes
		uint lenght = PHYSFS_fileLength(file);
		*buffer = new char[lenght];

		// Read data from a PhysicsFS firehandle. Returns a number of bytes read.
		uint bytes = PHYSFS_readBytes(file, *buffer, lenght);

		if (bytes != lenght)
		{
			LOG("%s", path, "ERROR: %s", PHYSFS_getLastError());
			RELEASE_ARRAY(buffer);
		}
		else
			ret = bytes;
	}
	else
		LOG("%s", path, "ERROR: %s", PHYSFS_getLastError());


	// Close a PhysicsFS firehandle
	PHYSFS_close(file);

	return ret;
}


int M_FileManager::CheckPath(const char* path)
{
	std::string string(path);

	if (pathVector.empty() == true)
	{
		pathVector.push_back(string);
		return -1;
	}

	int numBuffers = pathVector.size();
	for (int i = 0; i < numBuffers; i++)
	{
		if (pathVector[i] == path)
		{
			return i;
		}
	}

	pathVector.push_back(string);
	return -1;
}


// Add a new zip file or folder
bool M_FileManager::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}

// Check if a file exists
bool M_FileManager::Exists(const char* file) const
{
	return PHYSFS_exists(file) != 0;
}

bool M_FileManager::CreateDir(const char* dir)
{
	if (IsDirectory(dir) == false)
	{
		PHYSFS_mkdir(dir);
		return true;
	}
	return false;
}

// Check if a file is a directory
bool M_FileManager::IsDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file) != 0;
}

const char* M_FileManager::GetWriteDir() const
{
	//TODO: erase first annoying dot (".")
	return PHYSFS_getWriteDir();
}

void M_FileManager::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) const
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsDirectory(str.c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

void M_FileManager::GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) const
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++)
	{
		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);

		if (ext == extension)
			file_list.push_back(files[i]);
	}
}

PathNode M_FileManager::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext) const
{
	PathNode root;
	if (Exists(directory))
	{
		root.path = directory;
		std::string extension;
		App->physFS->SplitFilePath(directory, nullptr, nullptr, &root.localPath, &extension);
		if (root.localPath == "")
			root.localPath = directory;
		else
		{
			if (extension != "")
				root.localPath += "." + extension;
		}

		std::vector<std::string> file_list, dir_list;
		DiscoverFiles(directory, file_list, dir_list);

		//Adding all child directories
		for (uint i = 0; i < dir_list.size(); i++)
		{
			std::string str = directory;
			str.append("/").append(dir_list[i]);
			root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
		}
		//Adding all child files
		for (uint i = 0; i < file_list.size(); i++)
		{
			//Filtering extensions
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
			{
				filter = HasExtension(file_list[i].c_str(), *filter_ext);
			}
			if (ignore_ext != nullptr)
			{
				discard = HasExtension(file_list[i].c_str(), *ignore_ext);
			}
			if (filter == true && discard == false)
			{
				std::string str = directory;
				str.append("/").append(file_list[i]);
				root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
			}
		}
		root.isFile = HasExtension(root.path.c_str());
		root.isLeaf = root.children.empty() == true;
	}
	return root;
}

std::string M_FileManager::FindInPathNode(const char* toFind, PathNode& searchIn)
{
	for (int i = 0; i < searchIn.children.size(); i++)
	{
		size_t findPos = searchIn.children[i].path.find(toFind);

		if (findPos < searchIn.children[i].path.length())
		{
			return searchIn.children[i].path;
		}

		std::string searchInChilds;
		searchInChilds = FindInPathNode(toFind, searchIn.children[i]);

		if (searchInChilds != "")
			return searchInChilds.c_str();
	}

	return "";
}

R_TYPE M_FileManager::GetTypeFromExtension(const char* path)
{
	std::string extension = LowerCaseString(path);;

	if (extension == MODELS_EXTENSION)
	{
		return R_TYPE::MODEL;
	}
	else if (extension == TEXTURES_EXTENSION)
	{
		return R_TYPE::TEXTURE;
	}
	else if (extension == MATERIAL_EXTENSION)
	{
		return R_TYPE::MATERIAL;
	}
	else if (extension == SCENES_EXTENSION)
	{
		return R_TYPE::SCENE;
	}
	else if (extension == MESH_EXTENSION)
	{
		return R_TYPE::MESH;
	}
	else if (extension == SCRIPT_EXTENSION) 
	{
		return R_TYPE::SCRIPT;
	}

	return R_TYPE::NO_TYPE;
}

std::string M_FileManager::GetInternalExtensionFromType(R_TYPE type)
{
	std::string ret = ".";

	if (type == R_TYPE::MODEL)
	{
		ret += MODELS_EXTENSION;
	}
	else if (type == R_TYPE::TEXTURE)
	{
		ret += TEXTURES_EXTENSION;
	}
	else if (type == R_TYPE::MATERIAL)
	{
		ret += MATERIAL_EXTENSION;
	}
	else if (type == R_TYPE::SCENE)
	{
		ret += SCENES_EXTENSION;
	}
	else if (type == R_TYPE::MESH)
	{
		ret += MESH_EXTENSION;
	}
	else if (type == R_TYPE::SCRIPT) 
	{
		ret += SCRIPT_EXTENSION;
	}

	return ret;
}

std::string M_FileManager::GetExtensionFolderLibraryFromType(R_TYPE type)
{
	std::string ret = "";

	if (type == R_TYPE::MODEL)
	{
		ret = LIBRARY_MODELS;
	}
	else if (type == R_TYPE::TEXTURE)
	{
		ret = LIBRARY_TEXTURES;
	}
	else if (type == R_TYPE::MATERIAL)
	{
		ret = LIBRARY_MATERIALS;
	}
	else if (type == R_TYPE::SCENE)
	{
		ret = LIBRARY_SCENES;
	}
	else if (type == R_TYPE::MESH)
	{
		ret = LIBRARY_MESHES;
	}
	else if (type == R_TYPE::SCRIPT) 
	{
		ret = LIBRARY_SCRIPTS;
	}

	return ret;
}

bool M_FileManager::IsMeta(std::string& path)
{
	if (path.find("meta") < path.length())
		return true;

	return false;
}

std::string M_FileManager::RemoveCharFromString(std::string phrase, char param)
{
	std::string newPhrase;

	for (int i = 0; i < phrase.size(); ++i)
	{
		if (phrase[i] != param)
			newPhrase += phrase[i];
	}

	return newPhrase;

}

void M_FileManager::GetRealDir(const char* path, std::string& output) const
{
	output = PHYSFS_getBaseDir();

	std::string baseDir = PHYSFS_getBaseDir();
	std::string searchPath = *PHYSFS_getSearchPath();
	std::string realDir = PHYSFS_getRealDir(path);

	output.append(*PHYSFS_getSearchPath()).append("/");
	output.append(PHYSFS_getRealDir(path)).append("/").append(path);
}

std::string M_FileManager::GetPathRelativeToAssets(const char* originalPath) const
{
	std::string ret;
	GetRealDir(originalPath, ret);

	return ret;
}

bool M_FileManager::HasExtension(const char* path) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, nullptr, &ext);
	return ext != "";
}

bool M_FileManager::HasExtension(const char* path, std::string extension) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext == extension;
}

bool M_FileManager::HasExtension(const char* path, std::vector<std::string> extensions) const
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, nullptr, &ext);
	if (ext == "")
		return true;
	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
			return true;
	}
	return false;
}

std::string M_FileManager::NormalizePath(const char* full_path) const
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}

std::string M_FileManager::LowerCaseString(const char* path) const
{
	std::string newPath(path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		newPath[i] = std::tolower(newPath[i]);
	}
	return newPath;
}

void M_FileManager::SplitFilePath(const char* full_path, std::string* projectPath, std::string* path, std::string* file, std::string* extension) const
{
	if (full_path != nullptr)
	{
		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t projectSeparator = full.find("CollyraEngine/");
		size_t projectSeparator2 = full.find("CollyraEngine/Engine");
		size_t pos_dot = full.find_last_of(".");

		if (projectPath != nullptr)
		{
			if (projectSeparator < full.length())
			{
				if (projectSeparator2 < full.length())
					*projectPath = full.substr(projectSeparator2 + 21);
				else
					*projectPath = full.substr(projectSeparator + 14);

			}
			else
				projectPath->clear();
		}

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				*path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			else
				*file = full.substr(0, pos_dot);
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				*extension = full.substr(pos_dot + 1);
			else
				extension->clear();
		}
	}
}

std::string M_FileManager::GetProjectPathFromInternalRelative(std::string& fileProjectPath, std::string& fileRelativePath) const
{
	//Normalize all the paths------------
	fileRelativePath = NormalizePath(fileRelativePath.c_str());
	fileProjectPath = NormalizePath(fileProjectPath.c_str());

	//Get how many folders we want to erase-----
	uint foldersToGoBack = 0;
	for (int i = 0; i + 2 < fileRelativePath.size(); ++i)
	{
		if (fileRelativePath[i] == '.' && fileRelativePath[i + 1] == '.' && fileRelativePath[i + 2] == '/')
			foldersToGoBack++;
	}

	//We delete the "../" from the relative path
	fileRelativePath = fileRelativePath.substr(foldersToGoBack * 3);

	//If the project relative path starts with "/Assets", we delete the "/". Should not happen, just in case
	if (fileProjectPath[0] != '/')
		fileProjectPath = "/" + fileProjectPath;


	//Location of the first folder,----
	//If the path ends with "/" we make sure to start from there
	int toDeleteSince = -1;
	int toDeleteTo = -1;

	//How many folders we have in total
	int foldersToDelete = 0;
	for (int i = fileProjectPath.size() - 1; i >= 0; i--)
	{
		if (fileProjectPath[i] == '/')
		{

			if (toDeleteSince == -1)
				toDeleteSince = i;
			else
			{
				if (foldersToDelete < foldersToGoBack)
				{
					toDeleteTo = i;
					toDeleteSince = i;
				}
				foldersToDelete++;
			}


		}
	}

	//Final Cut--------
	if (foldersToDelete < foldersToGoBack)
	{
		LOG("Relative path is larger then the project folder tree. Proaable failure...")
	}
	else if (foldersToDelete == foldersToGoBack)
	{
		fileProjectPath = "";
	}
	else
	{
		fileProjectPath = fileProjectPath.substr(0, toDeleteTo + 1);
	}


	return std::string(fileProjectPath + fileRelativePath);
}

unsigned int M_FileManager::Load(const char* path, const char* file, char** buffer) const
{
	std::string full_path(path);
	full_path += file;
	return Load(full_path.c_str(), buffer);
}

bool M_FileManager::ImportFile(const char* file, std::string& relativePath)
{
	std::string fileStr, extensionStr;
	SplitFilePath(file, nullptr, nullptr, &fileStr, &extensionStr);
	std::string extensionFolder = GetExtensionFolderAssets(extensionStr.c_str()) + ("/");

	relativePath = relativePath.append(extensionFolder + fileStr.append(".") + extensionStr);

	return DuplicateFile(file, relativePath.c_str());
}

bool M_FileManager::DuplicateFile(const char* srcFile, const char* dstFile)
{
	bool ret = false;

	char buf[HUGE_STR];
	uint size;

	FILE* source = nullptr;
	fopen_s(&source, srcFile, "rb");

	if (source)
	{
		PHYSFS_file* dest = PHYSFS_openWrite(dstFile);

		if (source && dest)
		{
			while (size = fread_s(buf, HUGE_STR, 1, HUGE_STR, source))
				PHYSFS_write(dest, buf, 1, size);

			fclose(source);
			PHYSFS_close(dest);
			ret = true;

			LOG("File System copied file [%s] to [%s]", srcFile, dstFile);
		}
		else
			LOG("File System error while copy from [%s] to [%s]", srcFile, dstFile);
	}
	else
		LOG("File System error opening [%s], file not found or did not exist!", srcFile);



	return ret;
}

bool M_FileManager::DeleteFileIn(const char* srcFile)
{
	if (Exists(srcFile))
	{
		PHYSFS_delete(srcFile);
		return true;
	}

	return false;
}

std::string M_FileManager::GetExtensionFolderAssets(const char* fileExtension)
{
	std::string extension = LowerCaseString(fileExtension);;
	std::string ret = "";

	if (extension == "fbx")
	{
		ret = ASSETS_MODELS_PATH;
	}
	else if (extension == "png" || extension == "jpg" || extension == "dds")
	{
		ret = ASSETS_TEXTURES_PATH;
	}


	return ret;
}

std::string M_FileManager::GetExtensionFolderLibrary(const char* fileExtension)
{
	std::string extension = LowerCaseString(fileExtension);;
	std::string ret = "";

	if (extension == "fbx")
	{
		ret = LIBRARY_MODELS;
	}
	else if (extension == "png" || extension == "jpg" || extension == "dds" || extension == "tga")
	{
		ret = LIBRARY_TEXTURES;
	}
	else if (extension == "collscene")
	{
		ret = LIBRARY_SCENES;
	}
	else if (extension == "collmat")
	{
		ret = LIBRARY_MATERIALS;
	}
	else if (extension == "collscript") 
	{
		ret = LIBRARY_SCRIPTS;
	}

	return ret;
}

std::string M_FileManager::GetInternalExtension(const char* externalExtension)
{
	std::string extension = LowerCaseString(externalExtension);;

	if (extension == "fbx")
	{
		return std::string(MODELS_EXTENSION);
	}
	else if (extension == "png" || extension == "jpg" || extension == "dds" || extension == "tga")
	{
		return std::string(TEXTURES_EXTENSION);
	}
	else if (extension == "collscene")
	{
		return std::string(SCENES_EXTENSION);
	}
	else if (extension == "collmat")
	{
		return std::string(MATERIAL_EXTENSION);
	}
	else if (extension == "collscript") 
	{
		return std::string(SCRIPT_EXTENSION);
	}

	return std::string("unknown");
}

// Save a whole buffer to disk
uint M_FileManager::Save(const char* file, const void* buffer, unsigned int size, bool append) const
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG("[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true)
			{
				LOG("Added %u data to [%s%s]", size, GetWriteDir(), file);
			}
			else if (overwrite == true)
			{
				LOG("File [%s%s] overwritten with %u bytes", GetWriteDir(), file, size);
			}
			else
				LOG("New file created [%s%s] of %u bytes", GetWriteDir(), file, size);

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG("[error] File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG("[error] File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}

bool M_FileManager::Remove(const char* file)
{
	bool ret = false;

	if (file != nullptr)
	{
		//If it is a directory, we need to recursively remove all the files inside
		if (IsDirectory(file))
		{
			std::vector<std::string> containedFiles, containedDirs;
			PathNode rootDirectory = GetAllFiles(file);

			for (uint i = 0; i < rootDirectory.children.size(); ++i)
				Remove(rootDirectory.children[i].path.c_str());
		}

		if (PHYSFS_delete(file) != 0)
		{
			LOG("File deleted: [%s]", file);
			ret = true;
		}
		else
			LOG("File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError());
	}

	return ret;
}

uint64 M_FileManager::GetCurrDate(const char* filename)
{
	return PHYSFS_getLastModTime(filename);
}

std::string M_FileManager::GetUniqueName(const char* path, const char* name) const
{
	//TODO: modify to distinguix files and dirs?
	std::vector<std::string> files, dirs;
	DiscoverFiles(path, files, dirs);

	std::string finalName(name);
	bool unique = false;

	for (uint i = 0; i < 50 && unique == false; ++i)
	{
		unique = true;

		//Build the compare name (name_i)
		if (i > 0)
		{
			finalName = std::string(name).append("_");
			if (i < 10)
				finalName.append("0");
			finalName.append(std::to_string(i));
		}

		//Iterate through all the files to find a matching name
		for (uint f = 0; f < files.size(); ++f)
		{
			if (finalName == files[f])
			{
				unique = false;
				break;
			}
		}
	}
	return finalName;
}