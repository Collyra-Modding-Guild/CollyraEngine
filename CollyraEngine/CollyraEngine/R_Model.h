#ifndef __Resource_Model_H__
#define __Resource_Model_H__

#include "R_Resource.h"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "p2Defs.h"

struct ModelNode
{
	ModelNode(uint32 ID, const char* name, const float4x4& transform, uint64 parentID, int meshID = -1, int materialID = -1);

	std::string name;
	uint32 ID;
	float4x4 transform;

	uint32 parentID;
	int meshID;
	int materialID;
};

class R_Model : public Resource
{
public:
	R_Model(unsigned int uid);
	virtual ~R_Model();


	uint32 AddModelNode(uint32 ID, const char* name, const float4x4& transform, uint64 parentID, int meshID = -1, int materialID = -1);

public:
	std::vector<ModelNode> modelNodes;

};

#endif //__Resource_Model_H__