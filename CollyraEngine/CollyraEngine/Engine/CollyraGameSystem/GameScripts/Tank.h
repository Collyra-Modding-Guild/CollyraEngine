#pragma once

#include "../CollyraFramework.h"
#include "../../../CollObject.h"

class COLLYRAGAMESYSTEM_API Tank : public CollObject {

public:

	Tank();
	virtual ~Tank();

	void Start() override;
	void Update() override;

	void PlayerInputs();

private:

	C_Transform*	transform;
	Quat			rotation;
	float3			position;

	float			velocity;

};

COLLYRAGAMESYSTEM_EXPORTS Tank* CreateTank() {
	Tank* newTank = new Tank();
	return newTank;
}


