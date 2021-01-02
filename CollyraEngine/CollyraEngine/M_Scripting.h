#ifndef __ModuleScriptingInterface_H__
#define __ModuleScriptingInterface_H__

#include "Module.h"
#include <map>
#include <string>
#include <vector>

#define CPP_SCRIPT_TEMPLATE ".innerAssets/ScriptTemplateCPP.collTemplate"
#define H_SCRIPT_TEMPLATE ".innerAssets/ScriptTemplateH.collTemplate"
#define SCRIPT_PROJECT_PATH "CollyraGameSystem/CollyraGameSystem.vcxproj"
#define SCRIPT_PROJECTFILTERS_PATH "CollyraGameSystem/CollyraGameSystem.vcxproj.filters"
#define PROJECT_H_LABEL "hFiles"
#define PROJECT_CPP_LABEL "cppFiles"
#define VS_H_TEMPLATE "\t<ClInclude Include= \"GameScripts\\myFile.h\" />"
#define VS_CPP_TEMPLATE "\t<ClInclude Include= \"GameScripts\\myFile.cpp\" />"

typedef unsigned __int64 uint64;
typedef unsigned __int64 uint64;

class __declspec(dllexport) M_Scripting : public Module
{
public:
	M_Scripting(MODULE_TYPE type, bool startEnabled = true);
	~M_Scripting();

	bool Start();
	updateStatus Update(float dt);
	bool CleanUp();

	//Demo
	static void GameplayLog(const char* newLog);

	std::string CreateNewScript(const char* className);

	std::map<std::string, std::pair<std::string, std::string>>* GetClassesMap();

private:
	//Script files control-----------
	uint64 lastModDate;

	//Overall scripts that we are using (.cpp & .h)
	std::map<std::string, uint64> scriptFilesLoaded;

	//The Classes that we have loaded (key) & as value its related .cpp & .h (in that order)
	//Is is to control if someone changes the name of a file (we have the class associated) 
	// & if someone changes the class name (when the file is detected to be sabed, we check if the class name still the same)
	
	std::map<std::string, std::pair<std::string, std::string>> scriptClassLoaded;

};

#endif // __ModuleScriptingInterface_H__
