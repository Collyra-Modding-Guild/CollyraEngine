#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"

class M_Resources : public Module
{
public:

	M_Resources(MODULE_TYPE type, bool start_enabled = true);
	~M_Resources();

	bool Awake();


	bool CleanUp();

};

#endif // __ModuleResources_H__
