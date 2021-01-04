#pragma once

#include <string>
#include"SDL/include/SDL.h"
#include "MathGeoLib/include/MathGeoLib.h"

#define SCRIPTING "Game Scripting System: "

class GameObject;
class Component;
enum KEY_STATE;

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

	KEY_STATE GetKey(int id);
	KEY_STATE GetMouseButton(int id);
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

