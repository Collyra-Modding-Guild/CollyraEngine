#ifndef __ModuleScriptingInterface_H__
#define __ModuleScriptingInterface_H__

#include "Module.h"

typedef unsigned __int64 uint64;

class __declspec(dllexport) M_ScriptingInterface : public Module
{
public:
	M_ScriptingInterface(MODULE_TYPE type, bool startEnabled = true);
	~M_ScriptingInterface();

	bool Start();
	updateStatus Update(float dt);
	bool CleanUp();

	static void GameplayLog(const char* newLog);

	uint64 lastModDate;

};

#endif // __ModuleScriptingInterface_H__
