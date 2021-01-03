#pragma once

#include <string>
#include "MathGeoLib/include/MathGeoLib.h"

#define SCRIPTING "Game Scripting System: "

class GameObject;
class Component;

class __declspec(dllexport) CollObject
{
public:
	CollObject();
	virtual ~CollObject();

	virtual void Start() {}

	virtual void Update() {}
};


namespace  Debug 
{
	__declspec(dllexport) void Log(const char* text);

}

namespace Input 
{
	int GetMouseX();
	int GetMouseY();
	int GetMouseZ();
}

namespace gameObject 
{
	GameObject* CreateGameObject(std::string name = "", GameObject* parent = nullptr);

	GameObject* GetGameObject(unsigned int id);
	GameObject* GetGameObject(std::string name);

	bool DeleteGameObject(unsigned int id);
	bool DeleteGameObject(GameObject* gameObject);

	void DeleteCamera(Component* component);

}

