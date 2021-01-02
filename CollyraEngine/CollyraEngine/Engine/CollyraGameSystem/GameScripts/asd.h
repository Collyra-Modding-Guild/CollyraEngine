#pragma once

#include "../CollyraFramework.h"
#include "../../CollyraEngine/CollObject.h"

class COLLYRAGAMESYSTEM_API CollyraTemplate : public CollObject {

public:

	CollyraTemplate();
	virtual ~CollyraTemplate();

	void Start() override;
	void Update() override;

};

COLLYRAGAMESYSTEM_EXPORTS CollyraTemplate* CreateCollyraTemplate() {
	CollyraTemplate* newCollyraTemplate = new CollyraTemplate();
	return newCollyraTemplate;
}


