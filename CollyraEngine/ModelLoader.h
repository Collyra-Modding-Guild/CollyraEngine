#ifndef __ModelLoader_H__
#define __ModelLoader_H__

#include <vector>
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"

class JsonConfig;
class JsonArray;
class R_Model;
struct ModelNode;

namespace ModelLoader
{
	uint Save(const R_Model* rModel, char** buffer);
	void Load(R_Model* rModel, char* buffer);

	namespace Private
	{
		void SaveGameModel(const ModelNode* gameObject, JsonConfig saveTo, JsonArray* gameObjectsArray);
	}

}


#endif //__ModelLoader_H__