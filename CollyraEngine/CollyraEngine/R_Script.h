#ifndef __Resource_Script_H__
#define __Resource_Script_H__

#include "R_Resource.h"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "p2Defs.h"

class R_Script : public Resource
{
public:
	R_Script(unsigned int uid);
	virtual ~R_Script();
};

#endif //__Resource_Script_H_
