#ifndef __ModuleScriptingInterface_H__
#define __ModuleScriptingInterface_H__

#include "Module.h"

#define CPP_SCRIPT_TEMPLATE ".innerAssets/ScriptTemplateCPP.collTemplate"
#define H_SCRIPT_TEMPLATE ".innerAssets/ScriptTemplateH.collTemplate"

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

	void CreateNewScript(const char* className);

private:
	//Script files control-----------
	uint64 lastModDate;


};

#endif // __ModuleScriptingInterface_H__
