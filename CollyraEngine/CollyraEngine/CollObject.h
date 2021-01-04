#pragma once

#include <string>
#include"SDL/include/SDL.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "C_Transform.h"

#define SCRIPTING "Game Scripting System: "

class GameObject;
class Component;

enum INPUTS
{
	INPUT_IDLE = 0,
	INPUT_DOWN,
	INPUT_REPEAT,
	INPUT_UP
};

class __declspec(dllexport) CollObject
{
public:
	CollObject();
	virtual ~CollObject();

	virtual void Start() {}

	virtual void Update() {}

	void		SetMyGameObject(GameObject* gameObject);
	GameObject* GetMyGameObject() const;

private:
	GameObject* myGameObject;
};


namespace  Debug 
{
	__declspec(dllexport) void Log(const char* text);

}

namespace Input 
{
	__declspec(dllexport) int GetMouseX();
	__declspec(dllexport) int GetMouseY();
	__declspec(dllexport) int GetMouseZ();

	__declspec(dllexport) int GetKey(int id);
	__declspec(dllexport) int GetMouseButton(int id);
}

namespace gameObject 
{
	__declspec(dllexport) GameObject* CreateGameObject(std::string name = "", GameObject* parent = nullptr);

	__declspec(dllexport) GameObject* GetGameObject(unsigned int id);
	__declspec(dllexport) GameObject* GetGameObject(std::string name);

	__declspec(dllexport) bool DeleteGameObject(unsigned int id);
	__declspec(dllexport) bool DeleteGameObject(GameObject* gameObject);

	__declspec(dllexport) void DeleteCamera(Component* component);

}

