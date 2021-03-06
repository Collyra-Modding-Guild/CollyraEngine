#pragma once

#include <string>
#include "MathGeoLib/include/MathGeoLib.h"

#include "Globals.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Script.h"
#include "Timer.h"

#define SCRIPTING "Game Scripting System: "

#define DEBUG_LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

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

	virtual void OnEnable() {}

	virtual void OnDisable() {}

	void		SetMyGameObject(GameObject* gameObject);
	GameObject* GetMyGameObject() const;

private:
	GameObject* myGameObject;
};

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
	__declspec(dllexport) GameObject* CreateGameObject(const char* name = "", GameObject* parent = nullptr);

	__declspec(dllexport) GameObject* GetGameObject(unsigned int id);
	__declspec(dllexport) GameObject* GetGameObjectByName(const char* name);

	__declspec(dllexport) bool DeleteGameObject(unsigned int id);
	__declspec(dllexport) bool DeleteGameObject(GameObject* gameObject);

	__declspec(dllexport) void DeleteCamera(Component* component);

}

namespace Screen 
{
	__declspec(dllexport) int GetHeight();
	__declspec(dllexport) int GetWidth();
	__declspec(dllexport) LineSegment GetMouseWorldPosition(float2 mousePosition);
}

namespace Time
{
	__declspec(dllexport) float GetDeltaTime();
}


namespace Internal
{
	__declspec(dllexport) void LoadReflectVariable(std::string& varName, std::string& varType, void* varPtr, int size);
	__declspec(dllexport) void LoadSerializeVariable(std::string& varName, std::string& varType, void* varPtr, int size);
}
