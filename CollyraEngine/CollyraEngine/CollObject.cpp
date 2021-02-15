#include "Application.h"

#include "CollObject.h"
#include "GameObject.h"

#include "M_Scene.h"
#include "M_Input.h"
#include "M_Window.h"
#include "M_UIManager.h"
#include "M_Camera3D.h"
#include "M_Renderer3D.h"
#include "M_Scripting.h"

#include "Globals.h"


CollObject::CollObject() : myGameObject(nullptr)
{}

CollObject::~CollObject()
{}

void CollObject::SetMyGameObject(GameObject* gameObject)
{
	if (myGameObject == nullptr)
	{
		myGameObject = gameObject;
	}
	else
		LOG("You can not change the gameObject from this script.");
	
}

GameObject* CollObject::GetMyGameObject() const
{
	return myGameObject;
}

GameObject* gameObject::CreateGameObject(const char*  name, GameObject* parent)
{
	return App->scene->CreateGameObject(name, parent);
}

GameObject* gameObject::GetGameObject(unsigned int id)
{
	return App->scene->GetGameObject(id);
}

GameObject* gameObject::GetGameObjectByName(const char* name)
{
	return App->scene->GetGameObject(name);
}

bool gameObject::DeleteGameObject(unsigned int id)
{
	return App->scene->DeleteGameObject(id);
}

bool gameObject::DeleteGameObject(GameObject* gameObject)
{
	return App->scene->DeleteGameObject(gameObject);
}

void gameObject::DeleteCamera(Component* component)
{
	App->scene->DeleteCamera(component);
}

int Input::GetMouseX()
{
	return	App->uiManager->GetImGuiMousePosition().x;
}

int Input::GetMouseY()
{
	return	App->uiManager->GetImGuiMousePosition().y;
}

int Input::GetMouseZ()
{
	return App->input->GetMouseZ();
}

int Input::GetKey(int id)
{
	return App->input->GetKey(id);
}

int Input::GetMouseButton(int id)
{
	return App->input->GetMouseButton(id);
}

int Screen::GetHeight()
{
	return App->uiManager->GetSceneWindowSize().y;
}

int Screen::GetWidth()
{
	return App->uiManager->GetSceneWindowSize().x;
}

LineSegment Screen::GetMouseWorldPosition(float2 mousePosition)
{
	return App->camera->GetMouseWorldPosition(mousePosition, App->renderer3D->GetCurrentPlayCam());
}

float Time::GetDeltaTime()
{
	return App->GetGameDeltaTime();
}


void Internal::LoadReflectVariable(std::string& varName, std::string& varType, void* varPtr, int size)
{
	App->scriptInterface->LoadReflectableVariable(varName, varType, varPtr, size);
}