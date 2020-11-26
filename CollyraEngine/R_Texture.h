#ifndef __Resource_Texture_H__
#define __Resource_Texture_H__

#include "R_Resource.h"
#include "Globals.h"

class R_Texture : public Resource
{
public:
	R_Texture(unsigned int uid);
	virtual ~R_Texture();

public:
	uint textureId;

};

#endif //__Resource_Texture_H__