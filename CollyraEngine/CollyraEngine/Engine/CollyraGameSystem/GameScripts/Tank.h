#pragma once

#include "../CollyraFramework.h"
#include "../../../CollObject.h"

class COLLYRAGAMESYSTEM_API Tank : public CollObject 
{

public:

	Tank();
	virtual ~Tank();

	void Start() override;
	void Update() override;

	void PlayerInputs();
	Quat Rotation(float3 initialRotation, float3 rotation);

private:

	SERIALIZABLE
	(
		(bool) canMove,
		(float) spdMult
	);

	REFLECTABLE
	(
		(float) velocity
	);



	C_Transform*	transform;
	C_Transform*	turretTransform;
	GameObject*		myTurret;
	GameObject*		bulletToShoot;


};

COLLYRAGAMESYSTEM_EXPORTS Tank* CreateTank() {
	Tank* newTank = new Tank();
	ReflectVars(*newTank);
	SerializeVars(*newTank);
	return newTank;
}