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
	Quat Rotation(float3 initialRotation, float3 rotation);

private:

	C_Transform*	transform;
	C_Transform*	turretTransform;

	float			velocity;

	GameObject*		myTurret;

};

COLLYRAGAMESYSTEM_EXPORTS Tank* CreateTank() {
	Tank* newTank = new Tank();
	return newTank;
}


