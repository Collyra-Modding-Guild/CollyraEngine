#ifndef __Resource_Scene_H__
#define __Resource_Scene_H__

#include "R_Resource.h"


class GameObject;

class R_Scene : public Resource
{
public:
	R_Scene(unsigned int uid);
	virtual ~R_Scene();

	void SetUid(unsigned int newUid);

public:
	GameObject* root;

};

#endif //__Resource_Scene_H__