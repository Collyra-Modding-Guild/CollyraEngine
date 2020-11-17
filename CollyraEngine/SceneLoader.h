#ifndef __SceneLoader_H__
#define __SceneLoader_H__

#include <vector>
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;
class JsonConfig;
class JsonArray;
class Component;

namespace SceneLoader
{
	uint Import(const char* path);
	uint Save(const GameObject* hierarchyParent, char** buffer);
	uint Load(char* buffer, uint bufferSize);

	namespace Private
	{
		void SaveGameObject(const GameObject* gameObject, JsonConfig saveTo, JsonArray* gameObjectsArray);
		void SaveComponent(const Component* component, JsonConfig saveTo);
	}
}


#endif //__SceneLoader_H__