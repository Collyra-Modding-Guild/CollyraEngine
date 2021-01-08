#include "Bullet.h"

Bullet::Bullet() : CollObject(), myTransform(nullptr), speed(0), dir(0,0,0)
{
}

Bullet::~Bullet()
{
}

void Bullet::Start()
{
	destroyTimer.Start();
	myTransform = GetMyGameObject()->GetComponent<C_Transform>();
	speed = 2;
}

void Bullet::Update()
{
	myTransform->SetLocalTransformation(myTransform->GetPosition() + dir.Mul(speed),
		myTransform->GetRotation(),
		myTransform->GetScale());

	if (destroyTimer.ReadSec() > 2.0f)
	{
		this->GetMyGameObject()->Delete();
	}
}

void Bullet::SetDir(float3 newDirection)
{
	dir = newDirection;
}

COLLYRAGAMESYSTEM_EXPORTS Bullet* CreateBullet()
{
	Bullet* newBullet = new Bullet();
	return newBullet;
}
