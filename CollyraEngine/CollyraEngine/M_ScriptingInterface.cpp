#include "Application.h"
#include "M_ScriptingInterface.h"
#include "M_FileManager.h"


M_ScriptingInterface::M_ScriptingInterface(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), lastModDate(0)
{

}

M_ScriptingInterface::~M_ScriptingInterface()
{}


bool M_ScriptingInterface::Start()
{
	return true;
}

updateStatus M_ScriptingInterface::Update(float dt)
{
	//Demo ----------
	if (App->physFS->Exists("CollyraLibrary.cpp") != false)
		if (App->physFS->GetCurrDate("CollyraLibrary.cpp") != lastModDate)
		{
			FreeLibrary(App->gameSystemDll);
			App->gameSystemDll = 0;

			App->CompileDll();
			lastModDate = App->physFS->GetCurrDate("CollyraLibrary.cpp");
		}

	return UPDATE_CONTINUE;
}

bool M_ScriptingInterface::CleanUp()
{
	return true;
}

void M_ScriptingInterface::GameplayLog(const char* newLog)
{
	App->NewConsoleLog(newLog);
}
