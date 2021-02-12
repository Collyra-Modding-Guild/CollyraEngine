#include "Test.h"

Test::Test() : CollObject()
{
}

Test::~Test()
{
}

void Test::Start()
{
}

void Test::Update()
{
}

COLLYRAGAMESYSTEM_EXPORTS Test* CreateTest()
 {
	Test* newTest = new Test();
	return newTest;
}
