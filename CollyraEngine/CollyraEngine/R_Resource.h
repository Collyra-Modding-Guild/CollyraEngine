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
	MATERIAL,
	SCRIPT
};

class Resource
{
public:
	Resource(unsigned int uid, R_TYPE type);
	virtual ~Resource();

	void SetNameAndPath(std::string name, std::string path);

	void SetName(std::string name);
	void SetLibraryPath(std::string path);

	std::string GetLibraryPath() const;
	std::string GetName() const;

	unsigned int GetUid() const;
	R_TYPE GetType() const;

public:
	unsigned int referenceCount;

private:
	unsigned int uid;
	std::string libraryPath;
	std::string name;

	R_TYPE type;


};

#endif __Resource_H__