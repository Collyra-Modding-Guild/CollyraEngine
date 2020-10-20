#ifndef __TextureLoader_H__
#define __TextureLoader_H__

#include <vector>
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"


class Texture;

namespace TextureLoader
{
	void Init();
	void CleanUp();

	uint GetTextureID();
}


#endif //__TextureLoader_H__