#include "R_Resource.h"

Resource::Resource(unsigned int uid, R_TYPE type) : uid(uid), type(type), referenceCount(0)
{

}

Resource::~Resource()
{
}

void Resource::SetNameAndPath(std::string name, std::string path)
{
	SetName(name);
	SetLibraryPath(path);
}

void Resource::SetName(std::string name)
{
	this->name = name;
}


void Resource::SetLibraryPath(std::string path)
{
	libraryPath = path;
}

std::string Resource::GetLibraryPath() const
{
	return libraryPath;
}

std::string Resource::GetName() const
{
	return name;
}

R_TYPE Resource::GetType() const
{
	return type;
}

unsigned int Resource::GetUid() const
{
	return uid;
}
