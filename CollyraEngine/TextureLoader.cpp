#include "Application.h"
#include "TextureLoader.h"
#include "M_FileManager.h"

#include "OpenGL.h"

#include "Devil/include/ilut.h"
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
	else
		LOG("Devil Init!");

	return true;
}

void TextureLoader::CleanUp()
{}

uint TextureLoader::LoadDefaultTexture()
{
	uint texID = 1;

	GLubyte checkerImage[DEFAULT_TEXTURE_SIZE][DEFAULT_TEXTURE_SIZE][4];

	for (int i = 0; i < DEFAULT_TEXTURE_SIZE; i++)
	{
		for (int j = 0; j < DEFAULT_TEXTURE_SIZE; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DEFAULT_TEXTURE_SIZE, DEFAULT_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

uint TextureLoader::Load(const char* path)
{
	char* buffer = nullptr;

	uint bytesFile = App->physFS->Load(path, &buffer);

	if(bytesFile == 0)
		return 0;

	ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, bytesFile);
	uint ret = ilutGLBindTexImage();
	
	RELEASE_ARRAY(buffer);

	return ret;
}

