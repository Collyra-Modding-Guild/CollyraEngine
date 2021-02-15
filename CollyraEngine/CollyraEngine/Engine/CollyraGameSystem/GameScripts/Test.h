#pragma once

#include "../CollyraFramework.h"
#include "../../../CollObject.h"

class COLLYRAGAMESYSTEM_API Test : public CollObject {

public:

	Test();
	virtual ~Test();

	void Start() override;
	void Update() override;

};

COLLYRAGAMESYSTEM_EXPORTS Test* CreateTest();


