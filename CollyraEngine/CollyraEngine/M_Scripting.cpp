#include "Application.h"
#include "M_Scripting.h"
#include "M_FileManager.h"

#include <fstream> 
#include <sstream>
#include <iostream>

M_Scripting::M_Scripting(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), lastModDate(0)
{}

M_Scripting::~M_Scripting()
{}


bool M_Scripting::Start()
{
	lastModDate = App->physFS->GetCurrDate("GameScripts/CollyraLibrary.cpp");
	return true;
}

updateStatus M_Scripting::Update(float dt)
{
	//Demo ----------
	if (App->physFS->Exists("GameScripts/CollyraLibrary.cpp") != false)
		if (App->physFS->GetCurrDate("GameScripts/CollyraLibrary.cpp") != lastModDate)
		{
			FreeLibrary(App->gameSystemDll);
			App->gameSystemDll = 0;

			App->CompileDll();
			lastModDate = App->physFS->GetCurrDate("GameScripts/CollyraLibrary.cpp");
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

void M_Scripting::CreateNewScript(const char* className)
{
	//TODO: Compare & check if a className is already in use

	std::string saveToCpp = std::string(SCRIPT_FILES_LOCATION).append(className).append(".cpp");
	std::string saveToH = std::string(SCRIPT_FILES_LOCATION).append(className).append(".h");

	//Copy .cpp & .h
	App->physFS->DuplicateFile(CPP_SCRIPT_TEMPLATE, saveToCpp.c_str());
	App->physFS->DuplicateFile(H_SCRIPT_TEMPLATE, saveToH.c_str());

	//receive the code form the file path
	std::string headerCode = "", cppCode = "";
	std::ifstream hFile, cppFile;

	try
	{
		hFile.open(std::string("Engine/").append(saveToH).c_str());
		cppFile.open(saveToCpp.c_str());
	}

	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR SHADER NOT READABLE" << std::endl;
	}

	std::string line;
	while (std::getline(hFile, line))
	{
		while (line.find("CollyraTemplate") != std::string::npos)
		{
			line.replace(line.find("CollyraTemplate"), 16, std::string(className));
		}

		headerCode += line + std::string("\n");

	}
	hFile.close();
	cppFile.close();

	App->physFS->Save(saveToH.c_str(), headerCode.c_str(), headerCode.size());


}
