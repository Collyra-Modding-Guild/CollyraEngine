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
#define SCRIPTS_FOLDER "CollyraGameSystem/GameScripts/"
#define MAX_FILECHECK_PER_FRAME 3

//#define VS_H_TEMPLATE "\t<ClInclude Include=\"GameScripts\\myFile.h\" />"
//#define VS_CPP_TEMPLATE "\t<ClInclude Include=\"GameScripts\\myFile.cpp\" />"
//#define VS_HFILTER_TEMPLATE "\t<ClInclude Include=\"GameScripts\\myFile.h\" >\n\t  <Filter>Gameplay Scripts</Filter>\n\t</ClInclude>"
//#define VS_CPPFILTER_TEMPLATE "\t<ClInclude Include=\"GameScripts\\myFile.cpp\" >\n\t  <Filter>Gameplay Scripts</Filter>\n\t</ClInclude>"

typedef unsigned __int64 uint64;
class C_Script;

struct ScriptData
{
	ScriptData(std::string cppFile, std::string hFile, uint resourceId, const char* assetPath)
	{
		this->cppFile = cppFile;
		this->hFile = hFile;
		this->resourceId = resourceId;
		this->assetPath = assetPath;
	}

	std::string cppFile;
	std::string hFile;
	uint resourceId;
	const char* assetPath;
};

class __declspec(dllexport) M_Scripting : public Module
{
public:
	M_Scripting(MODULE_TYPE type, bool startEnabled = true);
	~M_Scripting();

	bool Awake() override;
	updateStatus Update(float dt);
	bool CleanUp();

	std::string CreateNewScript(const char* className);

	std::map<std::string, ScriptData>* GetClassesMap();
	std::map<std::string, uint64>* GetFileMap();

	uint GetScriptIdByClassName(const char* className);

	bool PerformHotReload();

	bool GetOnlineHotReload() const;
	void SetOnlineHotReload(bool newState);

	void LoadReflectableVariable(std::string varName, std::string varType, void* varPtr, int size);
	void LoadSerializeVariable(std::string varName, std::string varType, void* varPtr, int size);

private:
	bool CheckScriptStatus(const char* assetsPath, const char* libPath, unsigned int sciprtId);
	bool RefreshAllScripts();

public:
	C_Script* currentScriptLoading;

private:
	//Script files control-----------
	//Overall scripts that we are using (.cpp & .h)
	std::map<std::string, uint64> scriptFilesLoaded;

	//The Classes that we have loaded (key) & as value its related .cpp & .h (in that order)
	//Is is to control if someone changes the name of a file (we have the class associated) 
	// & if someone changes the class name (when the file is detected to be sabed, we check if the class name still the same)
	std::map<std::string, ScriptData> scriptClassLoaded;

	//Used to iterate the files (save in memory because we want to iterate few objects per frame)
	std::map<std::string, uint64>::iterator fileIterator;

	//Indicates if we will check every script file & hot reload instantly
	bool onlineHotReload;
};

#endif // __ModuleScriptingInterface_H__
