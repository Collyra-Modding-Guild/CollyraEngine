#include "Application.h"
#include "M_Scripting.h"
#include "M_FileManager.h"
#include "M_Resources.h"
#include "ScriptLoader.h"

#include <fstream> 
#include <sstream>
#include <iostream>

M_Scripting::M_Scripting(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), lastModDate(0)
{}

M_Scripting::~M_Scripting()
{}


bool M_Scripting::Start()
{
	//TODO: Get all the files (and their date) & all the clases, and throw them into the maps


	lastModDate = App->physFS->GetCurrDate("CollyraGameSystem/GameScripts/CollyraLibrary.cpp");
	return true;
}

updateStatus M_Scripting::Update(float dt)
{
	//Demo ----------
	if (App->physFS->Exists("CollyraGameSystem/GameScripts/CollyraLibrary.cpp") != false)
		if (App->physFS->GetCurrDate("CollyraGameSystem/GameScripts/CollyraLibrary.cpp") != lastModDate)
		{
			FreeLibrary(App->gameSystemDll);
			App->gameSystemDll = 0;

			App->CompileDll();
			lastModDate = App->physFS->GetCurrDate("CollyraGameSystem/GameScripts/CollyraLibrary.cpp");
		}

	return UPDATE_CONTINUE;
}

bool M_Scripting::CleanUp()
{
	return true;
}

void M_Scripting::GameplayLog(const char* newLog)
{
	App->NewConsoleLog(newLog);
}

std::string M_Scripting::CreateNewScript(const char* className)
{
	std::string saveToCpp = std::string(SCRIPT_FILES_LOCATION).append(className).append(".cpp");
	std::string saveToH = std::string(SCRIPT_FILES_LOCATION).append(className).append(".h");

	//Check if the script name already exists
	std::map<std::string, std::pair<std::string, std::string>>::iterator classIterator = scriptClassLoaded.find(className);
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

	LOG("Created new Script files with name %s", className);
	scriptClassLoaded.insert({ className , { saveToCpp, saveToH} });

	// Creating the Script Resource
	App->resources->ImportResourceFromAssets(
		ScriptLoader::Create(className, saveToCpp.c_str(), saveToH.c_str(), cppCode.c_str(), headerCode.c_str()).c_str()
	);

	//Add it to the Script Project

	std::string projectCode = "", projectFilters = "";
	std::ifstream projectFile, filtersFile;
	std::stringstream projectFileBuff, filtersFileBuff;

	projectFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	filtersFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		projectFile.open(SCRIPT_PROJECT_PATH, std::ifstream::in);
		filtersFile.open(SCRIPT_PROJECTFILTERS_PATH, std::ifstream::in);

		projectFileBuff << projectFile.rdbuf();
		filtersFileBuff << filtersFile.rdbuf();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "Exception opening file: " << std::strerror(errno) << "\n";
		LOG("Could not open the project file, please add the script manually");

		return "SUCCESS";
	}

	line = "";
	while (std::getline(projectFileBuff, line))
	{
		std::string add = "";
		if (line.find(PROJECT_H_LABEL) != std::string::npos)
		{
			std::string fileTemplate = VS_H_TEMPLATE;

			fileTemplate.replace(fileTemplate.find("myFile.h") , 8, std::string(className).append(".h"));

			add =  fileTemplate + "\n";
		}
		else if (line.find(PROJECT_CPP_LABEL) != std::string::npos)
		{
			std::string fileTemplate = VS_CPP_TEMPLATE;

			fileTemplate.replace(fileTemplate.find("myFile.cpp"), 10, std::string(className).append(".cpp"));

			add =  fileTemplate + "\n";
		}

		projectCode += line + "\n" + add;
	}

	App->physFS->Save(SCRIPT_PROJECT_PATH, projectCode.data(), projectCode.size());


	line = "";
	while (std::getline(filtersFileBuff, line))
	{
		std::string add = "";
		if (line.find("CollyraLibrary.h") != std::string::npos)
		{
			std::string fileTemplate = VS_HFILTER_TEMPLATE;

			fileTemplate.replace(fileTemplate.find("myFile.h"), 8, std::string(className).append(".h"));

			add = fileTemplate + "\n";
		}
		else if (line.find("CollyraLibrary.cpp") != std::string::npos)
		{
			std::string fileTemplate = VS_CPPFILTER_TEMPLATE;

			fileTemplate.replace(fileTemplate.find("myFile.cpp"), 10, std::string(className).append(".cpp"));

			add = fileTemplate + "\n";
		}

		projectFilters += add + line + "\n";
	}

	App->physFS->Save(SCRIPT_PROJECTFILTERS_PATH, projectFilters.data(), projectFilters.size());

	return "SUCCESS";
}

std::map<std::string, std::pair<std::string, std::string>>* M_Scripting::GetClassesMap()
{
	return &scriptClassLoaded;
}
