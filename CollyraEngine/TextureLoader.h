#ifndef __TextureLoader_H__
#define __TextureLoader_H__

#include <vector>
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"

class Texture;

namespace TextureLoader
{
	bool Init();
	void CleanUp();

	uint LoadDefaultTexture();
	uint Load(const char* path);

}


#endif //__TextureLoader_H__