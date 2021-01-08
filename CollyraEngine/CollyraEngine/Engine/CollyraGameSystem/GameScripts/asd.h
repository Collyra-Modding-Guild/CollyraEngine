#pragma once

#include "../CollyraFramework.h"
#include "../../../CollObject.h"

class COLLYRAGAMESYSTEM_API asd : public CollObject {

public:

	asd();
	virtual ~asd();

	void Start() override;
	void Update() override;

};

COLLYRAGAMESYSTEM_EXPORTS asd* Createasd();


