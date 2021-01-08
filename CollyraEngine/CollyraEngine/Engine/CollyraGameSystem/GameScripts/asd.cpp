#include "asd.h"

asd::asd() : CollObject()
{
}

asd::~asd()
{
}

void asd::Start()
{
}

void asd::Update()
{
}

COLLYRAGAMESYSTEM_EXPORTS asd* Createasd()
 {
	asd* newasd = new asd();
	return newasd;
}
