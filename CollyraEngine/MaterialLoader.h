#ifndef __MaterialLoader_H__
#define __MaterialLoader_H__

#include <vector>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class aiMaterial;
class R_Material;

namespace MaterialLoader
{
	//TODO: Meshpath should be included in resource material; that should be C_Material :S
	void Import(const aiMaterial* material, R_Material* myNewMaterial, const char* matName, const char* meshPath);
	uint Save(const R_Material* ourMaterial, char** buffer);
	void Load(R_Material* ourMaterial, char* buffer);
}


#endif //__MaterialLoader_H__