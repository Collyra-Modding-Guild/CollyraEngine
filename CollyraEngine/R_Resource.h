#ifndef __Resource_H__
#define __Resource_H__

#include <string>

enum class R_TYPE
{
	NO_TYPE = -1,

	MODEL,
	SCENE,
	MESH,
	TEXTURE,
	MATERIAL
};

class Resource
{

	Resource(unsigned int uid, R_TYPE type);
	virtual ~Resource();

private:
	unsigned int uid;
	std::string assetPath;
	std::string libraryPath;

	R_TYPE type;

	unsigned int referenceCount;

};

#endif __Resource_H__