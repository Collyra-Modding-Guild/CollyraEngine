#include "Application.h"
#include "M_ScriptingInterface.h"


M_ScriptingInterface::M_ScriptingInterface(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled)
{

}

M_ScriptingInterface::~M_ScriptingInterface()
{}


bool M_ScriptingInterface::Start()
{ return true; }

updateStatus M_ScriptingInterface::Update(float dt)
{ return UPDATE_CONTINUE; }

bool M_ScriptingInterface::CleanUp()
{ return true; }

void M_ScriptingInterface::NewGameplayLog(const char* log)
{
	// Call New Console LOG
	App->NewConsoleLog(log);
}
