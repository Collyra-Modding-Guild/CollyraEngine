#include "C_Script.h"
#include "GameObject.h"

#include "Application.h"
#include "M_Resources.h"
#include "M_Scripting.h"
#include "CollObject.h"

#include "R_Script.h"

C_Script::C_Script(bool active) : Component(COMPONENT_TYPE::SCRIPT, active), scriptId(0), myScript(nullptr), dataObject(nullptr)
{}

C_Script::~C_Script()
{
	App->resources->UnloadResource(scriptId);
	DeleteObjectData();
}

void C_Script::SetResourceId(uint newId)
{
	App->resources->UnloadResource(scriptId);

	DeleteObjectData();

	scriptId = newId;
	myScript = (R_Script*)App->resources->RequestResource(scriptId);

	GenerateObjectData();

}

int C_Script::GetResourceId() const
{
	return scriptId;
}

void C_Script::Start()
{
	if (App->gameClock->GameRunning() == false)
		return;

	if (dataObject != nullptr)
	{
		try
		{
			dataObject->Start();
		}
		catch (...)
		{
			LOG("SCRIPT SYSTEM ERROR: Could not find a script with name %s", myScript->GetScriptClassName());
		}
	}
}

void C_Script::Update(float dt)
{
	if (App->gameClock->GameRunning() == false)
		return;

	if (dataObject != nullptr)
	{
		try
		{
			dataObject->Update();
		}
		catch (...)
		{
			LOG("SCRIPT SYSTEM ERROR: Could not find a script with name %s", myScript->GetScriptClassName());
		}
	}

}

void C_Script::OnEnable()
{
	if (App->gameClock->GameRunning() == false)
		return;

	if (dataObject != nullptr)
	{
		try
		{
			dataObject->OnEnable();
		}
		catch (...)
		{
			LOG("SCRIPT SYSTEM ERROR: Could not find a script with name %s", myScript->GetScriptClassName());
		}
	}
}

void C_Script::OnDisable()
{
	if (App->gameClock->GameRunning() == false)
		return;

	if (dataObject != nullptr)
	{
		try
		{
			dataObject->OnDisable();
		}
		catch (...)
		{
			LOG("SCRIPT SYSTEM ERROR: Could not find a script with name %s", myScript->GetScriptClassName());
		}
	}
}

std::string C_Script::GetScriptClass()
{
	return myScript ? myScript->GetScriptClassName() : "No Script Loaded";
}

void C_Script::SetScriptClass(const char* newClass)
{
	uint classId = App->scriptInterface->GetScriptIdByClassName(newClass);

	if (classId != 0)
	{
		SetResourceId(classId);
	}
}

void C_Script::ResourceUpdated(std::map<uint, bool>* ids)
{
	if (scriptId == 0)
		return;

	std::map<uint, bool>::iterator it = ids->find(scriptId);

	if (it != ids->end())
	{
		if (it->second == true)
			SetResourceId(it->first);
		else
		{
			App->resources->UnloadResource(scriptId);
			myScript = nullptr;
			scriptId = 0;
		}
	}

}

void C_Script::DeleteObjectData(bool deleteReflectableVars)
{
	if (dataObject != nullptr)
	{
		delete dataObject;
		dataObject = nullptr;
	}

	serializedVariables.clear();

	if (deleteReflectableVars)
	{
		reflectableVariables.clear();
		for (int i = 0; i < prevReflectableVariables.size(); i++)
		{
			RELEASE(prevReflectableVariables[i].ptr);
		}
		prevReflectableVariables.clear();
	}


}

void C_Script::SaveReflectableVariables()
{
	for (int i = 0; i < prevReflectableVariables.size(); i++)
	{
		RELEASE(prevReflectableVariables[i].ptr);
	}
	prevReflectableVariables.clear();


	for (int i = 0; i < reflectableVariables.size(); i++)
	{
		char* varToCopy = (char*)reflectableVariables[i].ptr;

		int size = reflectableVariables[i].varSize;
		char* copyVar = new char[size];
		memcpy(copyVar, varToCopy, size);

		prevReflectableVariables.push_back({ reflectableVariables[i].name, reflectableVariables[i].type, copyVar, size });
	}
}

void C_Script::LoadReflectableVariables()
{
	for (int i = 0; i < reflectableVariables.size(); i++)
	{
		for (int y = 0; y < prevReflectableVariables.size(); y++)
		{
			if (prevReflectableVariables[y].varSize == reflectableVariables[i].varSize &&
				prevReflectableVariables[y].name == reflectableVariables[i].name
				&& prevReflectableVariables[y].type == reflectableVariables[i].type)
			{
				memcpy(reflectableVariables[i].ptr, prevReflectableVariables[y].ptr, prevReflectableVariables[y].varSize);
				break;
			}

		}
	}

	for (int i = 0; i < prevReflectableVariables.size(); i++)
	{
		RELEASE(prevReflectableVariables[i].ptr);
	}
	prevReflectableVariables.clear();
	reflectableVariables.clear();
}

void C_Script::GenerateObjectData()
{
	if (myScript != nullptr)
	{
		std::string buildFunction = std::string("Create" + std::string(myScript->GetScriptClassName()));
		void* (*Builder)() = (void* (*)())GetProcAddress(App->gameSystemDll, buildFunction.c_str());

		if (Builder != nullptr)
		{
			try
			{
				App->scriptInterface->currentScriptLoading = this;
				dataObject = (CollObject*)Builder();

				if (dataObject != nullptr)
				{
					dataObject->SetMyGameObject(GetGameObject());
				}

				if (App->gameClock->running == true)
				{
					dataObject->Start();
				}
			}
			catch (...)
			{
				LOG("SCRIPT SYSTEM ERROR: Could not find a script with name %s", myScript->GetScriptClassName());
			}
		}
	}
}

CollObject* C_Script::GetObjectData() const
{
	return dataObject;
}

bool C_Script::AddReflectVariable(std::string name, std::string type, void* ptr, int size)
{
	bool ret = false;

	if (!type.empty() && ptr != nullptr)
	{
		reflectableVariables.push_back({ name,type,ptr, size });
	}

	return ret;
}

bool C_Script::AddSerializeVariable(std::string name, std::string type, void* ptr, int size)
{
	bool ret = false;

	SERIALIZABLE_TYPE myType;

	if (type == "bool")
	{
		myType = SERIALIZABLE_TYPE::BOOL;
	}
	else
	{
		myType = SERIALIZABLE_TYPE::NO_TYPE;
	}


	if (!type.empty() && ptr != nullptr)
	{
		serializedVariables.push_back({ name,myType,ptr, size });
	}

	return ret;
}
