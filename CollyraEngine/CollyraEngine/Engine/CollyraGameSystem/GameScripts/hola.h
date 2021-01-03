#pragma once

#include "../CollyraFramework.h"
#include "../../../CollObject.h"

class COLLYRAGAMESYSTEM_API hola : public CollObject {

public:

	hola();
	virtual ~hola();

	void Start() override;
	void Update() override;

};

COLLYRAGAMESYSTEM_EXPORTS hola* Createhola() {
	hola* newhola = new hola();
	return newhola;
}


