#ifndef __ModuleScriptingInterface_H__
#define __ModuleScriptingInterface_H__

#include "Module.h"

class M_ScriptingInterface : public Module
{
public:
	M_ScriptingInterface(MODULE_TYPE type, bool startEnabled = true);
	~M_ScriptingInterface();

	bool Start();
	updateStatus Update(float dt);
	bool CleanUp();

	void NewGameplayLog(const char* log);

};

#endif // __ModuleScriptingInterface_H__
