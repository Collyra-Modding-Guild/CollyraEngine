#ifndef __TextureLoader_H__
#define __TextureLoader_H__

#include <vector>
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"

#define DEFAULT_TEXTURE_SIZE 40

class Texture;

namespace TextureLoader
{
	bool Init();
	void CleanUp();

	uint LoadDefaultTexture();

	uint Import(const char* path);
	uint Save(char** compressedBuffer, uint newTextureId);
	uint Load(char* buffer, uint bufferSize);

	//uint Load(const char* path);

}


#endif //__TextureLoader_H__