#include "Application.h"
#include "M_Scripting.h"
#include "M_FileManager.h"
#include "M_Resources.h"
#include "ScriptLoader.h"
#include "M_Scene.h"

#include "R_Script.h"
#include "Globals.h"

#include <fstream> 
#include <sstream>
#include <iostream>


M_Scripting::M_Scripting(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), onlineHotReload(false)
{}

M_Scripting::~M_Scripting()
{}


bool M_Scripting::Awake()
{
	RefreshAllScripts();
	return true;
}

bool M_Scripting::RefreshAllScripts()
{
	scriptFilesLoaded.clear();
	scriptClassLoaded.clear();

	//TODO: Get all the files (and their date) & all the clases, and throw them into the maps
	static std::vector<std::string> filter_ext = { "meta" };
	bool needResourceRefresh = false;
	PathNode allScripts = App->physFS->GetAllFiles(ASSETS_SCRIPTS_PATH, &filter_ext, nullptr);

	for (int i = 0; i < allScripts.children.size(); i++)
	{
		std::string assetsPath = "";
		std::string libPath = "";

		uint sciprtId = 0;

		App->resources->GetInfoFromMeta(allScripts.children[i].path.c_str(), &sciprtId, nullptr, nullptr, &libPath);
		assetsPath = allScripts.children[i].path;
		assetsPath.replace(assetsPath.find(".meta"), 5, "");

		bool scriptRefresh = CheckScriptStatus(assetsPath.c_str(), libPath.c_str(), sciprtId);

		if (scriptRefresh == true && needResourceRefresh == false)
			needResourceRefresh = true;
	}

	if (needResourceRefresh)
	{
		App->resources->NotifyHotReload();
	}

	fileIterator = scriptFilesLoaded.begin();

	return true;
}

updateStatus M_Scripting::Update(float dt)
{
	updateStatus ret = UPDATE_CONTINUE;

	//Hot reloading ----------
	if (onlineHotReload == false)
		return ret;

	if (scriptFilesLoaded.size() > 0)
		for (int i = 0; i < MAX_FILECHECK_PER_FRAME; i++)
		{
			if (fileIterator->second != App->physFS->GetCurrDate(fileIterator->first.c_str()))
			{

				PerformHotReload();
				return ret;
			}

			fileIterator++;

			if (fileIterator == scriptFilesLoaded.end())
				fileIterator = scriptFilesLoaded.begin();
		}

	return ret;
}

bool M_Scripting::CleanUp()
{
	return true;
}

std::string M_Scripting::CreateNewScript(const char* className)
{
	std::string saveToCpp = std::string(SCRIPT_FILES_LOCATION).append(className).append(".cpp");
	std::string saveToH = std::string(SCRIPT_FILES_LOCATION).append(className).append(".h");

	//Check if the script name already exists
	std::map<std::string, ScriptData>::iterator classIterator = scriptClassLoaded.find(className);
	if (classIterator != scriptClassLoaded.end())
	{
		return "Script already loaded in the Project!";
	}


	//Copy .cpp & .h
	if (!App->physFS->DuplicateFile(CPP_SCRIPT_TEMPLATE, saveToCpp.c_str()) ||
		!App->physFS->DuplicateFile(H_SCRIPT_TEMPLATE, saveToH.c_str()))
	{
		return "Files could not be created!";
	}


	//Get the code as a string
	std::string headerCode = "", cppCode = "";
	std::ifstream hFile, cppFile;
	std::stringstream headerCodeBuff, cppCodeBuff;

	hFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	cppFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		hFile.open(saveToH.c_str(), std::ifstream::in);
		cppFile.open(saveToCpp.c_str());

		headerCodeBuff << hFile.rdbuf();
		cppCodeBuff << cppFile.rdbuf();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "Exception opening file: " << std::strerror(errno) << "\n";
		LOG("Could not open newly created cpp or h file");
		App->physFS->DeleteFileIn(saveToCpp.c_str());
		App->physFS->DeleteFileIn(saveToH.c_str());

		return "Files could not be opened!";
	}

	//Substitute that string for the class name

	std::string line;
	while (std::getline(headerCodeBuff, line))
	{
		while (line.find("CollyraTemplate") != std::string::npos)
		{
			line.replace(line.find("CollyraTemplate"), 15, std::string(className));
		}

		headerCode += line + std::string("\n");
	}

	App->physFS->Save(saveToH.c_str(), headerCode.data(), headerCode.size());
	scriptFilesLoaded.insert({ saveToH , App->physFS->GetCurrDate(saveToH.c_str()) });

	while (std::getline(cppCodeBuff, line))
	{
		while (line.find("CollyraTemplate") != std::string::npos)
		{
			line.replace(line.find("CollyraTemplate"), 15, std::string(className));
		}

		cppCode += line + std::string("\n");
	}

	App->physFS->Save(saveToCpp.c_str(), cppCode.data(), cppCode.size());
	scriptFilesLoaded.insert({ saveToCpp , App->physFS->GetCurrDate(cppCode.c_str()) });


	// Creating the Script Resource
	std::string createScript = ScriptLoader::Create(className, saveToCpp.c_str(), saveToH.c_str(), cppCode.c_str(), headerCode.c_str()).c_str();

	uint scriptId = App->resources->ImportResourceFromAssets(createScript.c_str());

	LOG("Created new Script files with name %s", className);
	scriptClassLoaded.insert({ className , { saveToCpp, saveToH, scriptId,createScript.c_str()} });

	LOG("If you wish to use the script, please add it to the project in VS");

	//Add it to the Script Project

	//TODO: We failed, we wil get them next time :)
	{
		//std::string projectCode = "", projectFilters = "";
	//std::ifstream projectFile, filtersFile;
	//std::stringstream projectFileBuff, filtersFileBuff;

	//projectFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	//filtersFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	//try
	//{
	//	projectFile.open(SCRIPT_PROJECT_PATH, std::ifstream::in);
	//	filtersFile.open(SCRIPT_PROJECTFILTERS_PATH, std::ifstream::in);

	//	projectFileBuff << projectFile.rdbuf();
	//	filtersFileBuff << filtersFile.rdbuf();
	//}
	//catch (std::ifstream::failure e)
	//{
	//	std::cerr << "Exception opening file: " << std::strerror(errno) << "\n";
	//	LOG("Could not open the project file, please add the script manually");

	//	return "SUCCESS";
	//}

	//line = "";
	//while (std::getline(projectFileBuff, line))
	//{
	//	std::string add = "";
	//	if (line.find(PROJECT_H_LABEL) != std::string::npos)
	//	{
	//		std::string fileTemplate = VS_H_TEMPLATE;

	//		fileTemplate.replace(fileTemplate.find("myFile.h") , 8, std::string(className).append(".h"));

	//		add =  fileTemplate + "\n";
	//	}
	//	else if (line.find(PROJECT_CPP_LABEL) != std::string::npos)
	//	{
	//		std::string fileTemplate = VS_CPP_TEMPLATE;

	//		fileTemplate.replace(fileTemplate.find("myFile.cpp"), 10, std::string(className).append(".cpp"));

	//		add =  fileTemplate + "\n";
	//	}

	//	projectCode += line + "\n" + add;
	//}

	//App->physFS->Save(SCRIPT_PROJECT_PATH, projectCode.data(), projectCode.size());


	//line = "";
	//while (std::getline(filtersFileBuff, line))
	//{
	//	std::string add = "";
	//	if (line.find("CollyraLibrary.h") != std::string::npos)
	//	{
	//		std::string fileTemplate = VS_HFILTER_TEMPLATE;

	//		fileTemplate.replace(fileTemplate.find("myFile.h"), 8, std::string(className).append(".h"));

	//		add = fileTemplate + "\n";
	//	}
	//	else if (line.find("CollyraLibrary.cpp") != std::string::npos)
	//	{
	//		std::string fileTemplate = VS_CPPFILTER_TEMPLATE;

	//		fileTemplate.replace(fileTemplate.find("myFile.cpp"), 10, std::string(className).append(".cpp"));

	//		add = fileTemplate + "\n";
	//	}

	//	projectFilters += add + line + "\n";
	//}

	//App->physFS->Save(SCRIPT_PROJECTFILTERS_PATH, projectFilters.data(), projectFilters.size());

	}



	return "SUCCESS";
}

std::map<std::string, ScriptData>* M_Scripting::GetClassesMap()
{
	return &scriptClassLoaded;
}

uint M_Scripting::GetScriptIdByClassName(const char* className)
{
	uint ret = 0;

	std::map<std::string, ScriptData>::iterator classIterator = scriptClassLoaded.find(className);
	if (classIterator != scriptClassLoaded.end())
	{
		ret = classIterator->second.resourceId;
	}

	return ret;
}

bool M_Scripting::PerformHotReload()
{
	//TODO: Update all files mod Date & Hot - Reload
	// Hot-reload consists of: 
		//Saving the data in a tempral folder  -> PRE
		// Deleting all the previous classes -> PRE
		// Re-compiling the code -> App.CompileDll()
		// Perform a check (just as the start) (before or after ?) !!!!!!!!!!!!!!!!!!!! -> RefreshScripts()
		// Re-load the changes: -> POST
			// Check if the class still exists (the resource will be re-created, if exists, all good)
			// If exists, we create it & we capture the variables that have been created
			// Then we compare them with the ones we currently have (if there is a match, we substitute)

	//TODO: Call onDisable, onEnable when go are diabled/enabled & Start when scene starts (scripts); also update only if playing
	//TODO: Make camera change on play
	//TODO: Save reloadable vars & inspector from the script

	//We can only refresh if we are sure that the dll has compiled succesfully, but still need to previous dll to delete & store
	bool ret = App->CompileDll(false, false);

	if (ret == true)
	{
		RefreshAllScripts();
		App->scene->PrerHotReload();

		bool copyOk = App->CopyNewDll();
		if (copyOk == true)
		{
			App->scene->PostrHotReload();
		}

	}

	return ret;
}

bool M_Scripting::GetOnlineHotReload() const
{
	return onlineHotReload;
}

void M_Scripting::SetOnlineHotReload(bool newState)
{
	onlineHotReload = newState;
}

bool M_Scripting::CheckScriptStatus(const char* assetsPath, const char* libPath, unsigned int sciprtId)
{
	R_Script buffer(0);
	bool needsReWrite = false;

	char* fileBuffer = nullptr;
	uint size = App->physFS->Load(libPath, &fileBuffer);
	if (size > 0)
	{
		ScriptLoader::Load(&buffer, fileBuffer);
		RELEASE(fileBuffer);

		//Check files exist
		if (!App->physFS->Exists(buffer.GetScriptCppPath()) || !App->physFS->Exists(buffer.GetScriptHPath()))
		{
			LOG("Detected that script with class %s has no code files in the project. Searching for them. . .", libPath);

			std::vector<std::string> scriptFiles, folders;
			App->physFS->DiscoverFiles(SCRIPTS_FOLDER, scriptFiles, folders);

			for (int i = 0; i < scriptFiles.size(); i++)
			{
				if (scriptFiles[i].find(".h") < scriptFiles[i].length())
				{
					std::string headerCode = "";
					std::ifstream hFile;
					std::stringstream headerCodeBuff;

					hFile.open(std::string(SCRIPTS_FOLDER).append(scriptFiles[i]).c_str());

					headerCodeBuff << hFile.rdbuf();

					hFile.close();

					headerCode = headerCodeBuff.str();

					std::string createSearch = std::string("Create" + std::string(buffer.GetScriptClassName()));

					if (headerCode.find(createSearch) < headerCode.length())
					{
						LOG("Found new files! %s", scriptFiles[i].c_str());

						std::string cppFile = std::string(SCRIPTS_FOLDER).append(scriptFiles[i]);

						if (!App->physFS->Exists(cppFile.replace(cppFile.find(".h"), 2, std::string(".cpp")).c_str()))
						{
							LOG("Could not find script cpp file, deleting script. . .");
							App->physFS->DeleteFileIn(assetsPath);
							App->resources->CheckAssetInMeta(std::string(assetsPath) + ".meta", assetsPath);
							return false;
						}

						buffer.SetScriptCppPath(cppFile.c_str());
						buffer.SetScriptHPath(std::string(SCRIPTS_FOLDER).append(scriptFiles[i]).c_str());

						buffer.SetScriptCppFileModDate(App->physFS->GetCurrDate(buffer.GetScriptCppPath()));
						buffer.SetScriptHFileModDate(App->physFS->GetCurrDate(buffer.GetScriptHPath()));

						buffer.SetScriptHCode(headerCode.c_str());

						hFile.open(cppFile.c_str());
						headerCodeBuff << hFile.rdbuf();
						hFile.close();
						buffer.SetScriptCppCode(headerCode.c_str());

						needsReWrite = true;
						break;
					}
				}
			}

			if (needsReWrite == false)
			{
				LOG("Could not find script files, re-creating them from the asset information ");
				std::string hCode = buffer.GetScriptHCode();
				App->physFS->Save(buffer.GetScriptHPath(), hCode.data(), hCode.size());

				std::string cppCode = buffer.GetScriptCppCode();
				App->physFS->Save(buffer.GetScriptCppPath(), cppCode.data(), cppCode.size());
			}
		}

		//If a script .cpp & .h don't exist, we create them (for agile projects, we create from assets)

		//Check if class name is the same
		{
			std::string headerCode = "";
			std::ifstream hFile;
			std::stringstream headerCodeBuff;

			hFile.open(buffer.GetScriptHPath());
			headerCodeBuff << hFile.rdbuf();
			hFile.close();
			headerCode = headerCodeBuff.str();

			std::string createSearch = std::string("Create" + std::string(buffer.GetScriptClassName()) + "()");

			if (headerCode.find(createSearch) > headerCode.length())
			{
				LOG("Class Name Changed in %s ", buffer.GetScriptHPath());
				std::string newname = "";

				uint exporterPos = headerCode.find("COLLYRAGAMESYSTEM_EXPORTS");
				if (exporterPos < headerCode.length())
				{
					uint endNamePos = headerCode.find(std::string("Create"), exporterPos);
					newname = headerCode.substr(exporterPos + 26, endNamePos - exporterPos - 28);
					LOG("Asumming script new name is %s", newname.c_str());
				}
				else
				{
					LOG("Could not find exports, deleting script . . .");
					App->physFS->DeleteFileIn(assetsPath);
					App->resources->CheckAssetInMeta(std::string(assetsPath) + ".meta", assetsPath);
					return false;
				}

				buffer.SetScriptClassName(newname.c_str());

				needsReWrite = true;
			}

			if (needsReWrite == true)
			{
				size = ScriptLoader::Save(&buffer, &fileBuffer);;

				App->physFS->Save(libPath, fileBuffer, size);
				App->physFS->Save(assetsPath, fileBuffer, size);

				RELEASE(fileBuffer);
			}
		}

	}

	scriptFilesLoaded.insert({ buffer.GetScriptCppPath(), App->physFS->GetCurrDate(buffer.GetScriptCppPath()) });
	scriptFilesLoaded.insert({ buffer.GetScriptHPath(), App->physFS->GetCurrDate(buffer.GetScriptHPath()) });

	scriptClassLoaded.insert({ buffer.GetScriptClassName() , {buffer.GetScriptCppPath(), buffer.GetScriptHPath(), sciprtId, assetsPath} });

	return needsReWrite;
}

