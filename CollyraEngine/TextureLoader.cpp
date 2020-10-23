#include "Application.h"
#include "TextureLoader.h"
#include "M_FileManager.h"

#include "p2Defs.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Devil/include/ilut.h"

#pragma comment (lib, "glu32.lib")   
#pragma comment (lib, "opengl32.lib") 
#pragma comment (lib, "Glew/libx86/glew32.lib")


#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )


bool TextureLoader::Init()
{
	//Initialize DevIL
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	// ilClearColour(255, 255, 255, 000);

	//Check for error
	ILenum ilError = ilGetError();
	if (ilError != IL_NO_ERROR)
	{
		printf("Error initializing DevIL! %s\n", iluErrorString(ilError));
		return false;
	}

	return true;
}

void TextureLoader::CleanUp()
{}

uint TextureLoader::LoadDefaultTexture()
{
	uint textID = 1;

	GLubyte checkerImage[20][20][4];

	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textID);
	glBindTexture(GL_TEXTURE_2D, textID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 20, 20, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	return textID;
}

uint TextureLoader::Load(const char* path)
{
	uint Image = 0;

	ilGenImages(1, &Image);
	ilBindImage(Image);

	//-------------
	ilLoadImage(path);

/*	ILinfo ImageInfo;
	ImageInfo.Id = Image;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}*/

	uint ret = ilutGLBindTexImage();
	ilDeleteImages(1, &Image);

	//-----------------------
	//char* buffer = nullptr;

	//uint bytesFile = App->physFS->Load(path, &buffer);

	//ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, bytesFile);
	//uint ret = ilutGLBindTexImage();

	//RELEASE_ARRAY(buffer);

	return ret;
}

