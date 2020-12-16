#ifndef __Resource_Material_H__
#define __Resource_Material_H__

#include "R_Resource.h"
#include "Color.h"
#include "Globals.h"

class R_Texture;

class R_Material : public Resource
{
public:
	R_Material(unsigned int uid);
	virtual ~R_Material();

	void SetColor(Color color);

	void SetTextureResourceId(uint rTexId, bool load = true);

	Color GetColor() const;
	int GetTextureResourceId() const;
	int GetTextureId() const;
	
	std::string GetTextureName() const;
	std::string GetTexturePath() const;

private:
	int idTextureResource;
	Color color;

	R_Texture* myTexResource;
};

#endif //__Resource_Material_H__