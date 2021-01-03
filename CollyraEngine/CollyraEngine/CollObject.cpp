#include "Application.h"

#include "CollObject.h"
#include "GameObject.h"

#include "M_Scene.h"
#include "M_Input.h"

#include "Globals.h"


CollObject::CollObject()
{}

CollObject::~CollObject()
{}

void Debug::Log(const char* text)
{
	LOG(std::string(SCRIPTING).append(text).c_str());
}

GameObject* gameObject::CreateGameObject(std::string name, GameObject* parent)
{
	return App->scene->CreateGameObject(name, parent);
}

GameObject* gameObject::GetGameObject(unsigned int id)
{
	return App->scene->GetGameObject(id);
}

GameObject* gameObject::GetGameObject(std::string name)
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
	return App->input->GetMouseX();
}

int Input::GetMouseY()
{
	return App->input->GetMouseY();
}

int Input::GetMouseZ()
{
	return App->input->GetMouseZ();
}
