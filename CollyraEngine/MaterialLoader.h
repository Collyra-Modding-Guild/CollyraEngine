#ifndef __MaterialLoader_H__
#define __MaterialLoader_H__

#include <vector>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class aiMaterial;
class C_Material;

namespace MaterialLoader
{
	//TODO: Meshpath should be included in resource material; that should be C_Material :S
	void IImport(const aiMaterial* material, C_Material* myNewMaterial, const char* meshPath);
	std::string Save(const C_Material* ourMaterial);
	void Load(C_Material* ourMaterial, const char* path);
}


#endif //__MaterialLoader_H__