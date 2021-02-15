#pragma once

#include "../CollyraFramework.h"
#include "../../../CollObject.h"

class COLLYRAGAMESYSTEM_API Bullet : public CollObject 
{

public:

	Bullet();
	virtual ~Bullet();

	void Start() override;
	void Update() override;

	void SetDir(float3 newDirection);

public:

	REFLECTABLE
	(
		(float)speed,
		(float3)dir
	);


	Timer destroyTimer;
	C_Transform* myTransform;

};

COLLYRAGAMESYSTEM_EXPORTS Bullet* CreateBullet();


