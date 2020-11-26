#ifndef __TextureLoader_H__
#define __TextureLoader_H__

#include <vector>
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"

#define DEFAULT_TEXTURE_SIZE 40

class R_Texture;

namespace TextureLoader
{
	bool Init();
	void CleanUp();

	uint LoadDefaultTexture();

	uint Import(const char* path, char** buffer, unsigned int bufferSize);
	uint Save(char** compressedBuffer);
	uint Load(char* buffer, uint bufferSize, R_Texture* rTexture);

	//uint Load(const char* path);

}


#endif //__TextureLoader_H__