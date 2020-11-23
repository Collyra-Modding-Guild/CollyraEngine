#include "R_Resource.h"

Resource::Resource(unsigned int uid, R_TYPE type) : uid(uid), type(type), referenceCount(0)
{}

Resource::~Resource()
{
}
