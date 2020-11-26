#include "R_Model.h"


R_Model::R_Model(unsigned int uid) : Resource(uid, R_TYPE::MODEL)
{}


R_Model::~R_Model()
{}

uint32 R_Model::AddModelNode(uint32 ID, const char* name, const float4x4& transform, uint64 parentID, int meshID, int materialID)
{
	modelNodes.push_back({ID, name, transform, parentID, meshID, materialID});

	return ID;
}


ModelNode::ModelNode(uint32 ID, const char* name, const float4x4& transform, uint64 parentID, int meshID, int materialID) :
	ID(ID), name(name), parentID(parentID), materialID(materialID), meshID(meshID), transform(transform)
{}
