#ifndef __Resource_Material_H__
#define __Resource_Material_H__

#include "R_Resource.h"
#include "Color.h"
#include "Globals.h"

class R_Material : public Resource
{
public:
	R_Material(unsigned int uid);
	virtual ~R_Material();

	void SetColor(Color color);
	void SetNameAndPath(const char* name, const char* path);

	void SetTextureResourceId(uint rTexId);

	std::string GetName() const;
	std::string GetPath() const;

	Color GetColor() const;
	int GetTextureId() const;
	
	void SetTextureId(int newId);

private:
	int idTextureResource;
	Color color;
	std::string name;
	std::string path;

};

#endif //__Resource_Material_H__