#include "C_Script.h"
#include "GameObject.h"

#include "Application.h"
#include "M_Resources.h"
#include "M_Scripting.h"
#include "CollObject.h"

#include "R_Script.h"

C_Script::C_Script(bool active) : Component(COMPONENT_TYPE::SCRIPT, active), scriptId(0), script(nullptr), dataObject(nullptr)
{}

C_Script::~C_Script()
{ }

void C_Script::SetResourceId(uint newId)
{
	App->resources->UnloadResource(scriptId);

	if (dataObject != nullptr)
	{
		RELEASE(dataObject);
		dataObject = nullptr;
	}

	scriptId = newId;
	script = (R_Script*)App->resources->RequestResource(scriptId);

	if (script != nullptr)
	{
		std::string buildFunction = std::string("Create" + std::string(script->GetScriptClassName()));
		void* (*Builder)() = (void* (*)())GetProcAddress(App->gameSystemDll, buildFunction.c_str());

		if (Builder != nullptr)
		{
			try
			{
				dataObject = (CollObject*)Builder();
			}
			catch (...)
			{
				LOG("SCRIPT SYSTEM ERROR: Could not find a script with name %s", script->GetScriptClassName());
			}
		}

	}

}

int C_Script::GetResourceId() const
{
	return scriptId;
}

void C_Script::Update(float dt)
{
	if (dataObject != nullptr)
	{
		dataObject->Update();
	}

}

std::string C_Script::GetScriptClass()
{
	return script ? script->GetScriptClassName() : "No Script Loaded";
}

void C_Script::SetScriptClass(const char* newClass)
{
	uint classId = App->scriptInterface->GetScriptIdByClassName(newClass);

	if (classId != 0)
	{
		SetResourceId(classId);
	}
}
