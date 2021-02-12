#include "Tank.h"
#include "Bullet.h"

Tank::Tank() : CollObject(), velocity(0.0f), myTurret(nullptr), bulletToShoot(nullptr), transform(nullptr), turretTransform(nullptr)
{
}

Tank::~Tank()
{
}

void Tank::Start()
{
	transform = GetMyGameObject()->GetComponent<C_Transform>();

	myTurret = gameObject::GetGameObjectByName("TankTurret");
	turretTransform = myTurret->GetComponent<C_Transform>();

	bulletToShoot = gameObject::CreateGameObject("Bullet");
	C_Mesh* cMesh = (C_Mesh*)bulletToShoot->CreateComponent(COMPONENT_TYPE::MESH);
	cMesh->SetResourceIdFromName("cube");

	C_Script* bullScript = (C_Script*)bulletToShoot->CreateComponent(COMPONENT_TYPE::SCRIPT);
	bullScript->SetScriptClass("Bullet");

	bulletToShoot->SetActive(false);

}

void Tank::Update()
{
	PlayerInputs();
}

void Tank::PlayerInputs()
{
	float3 forward = transform->GetForward();

	if (Input::GetKey(SDL_SCANCODE_O) == INPUT_REPEAT)
	{
		velocity = 10.0f;
	}
	else if (Input::GetKey(SDL_SCANCODE_S) == INPUT_REPEAT)
	{
		velocity = -10.0f;
	}
	else
		velocity = 0;


	float3 rotation = transform->GetRotationEuler();
	float3 initialRot = rotation;

	if (Input::GetKey(SDL_SCANCODE_A) == INPUT_REPEAT)
	{
		rotation.y += 1;
	}
	else if (Input::GetKey(SDL_SCANCODE_D) == INPUT_REPEAT)
	{
		rotation.y -= 1;
	}

	DEBUG_LOG("%f", Time::GetDeltaTime());


	transform->SetLocalTransformation(transform->GetPosition() + forward * velocity * Time::GetDeltaTime(), // Tank Position
		Rotation(initialRot, rotation),				 // Tank Rotation
		transform->GetScale());						 // Tank Scale


	//Turret Rotation-------------------------------------

	float3 turRotation = turretTransform->GetRotationEuler();
	float3 turInitialRot = turRotation;

	LineSegment ray = Screen::GetMouseWorldPosition({ (float)Input::GetMouseX() , (float)Input::GetMouseY() });

	float3 lookAt = { ray.b.x - turretTransform->GetPosition().x ,0,  ray.b.z - turretTransform->GetPosition().z };
	lookAt.Normalize();

	float3 turrForward = { turretTransform->GetForward().x,0, turretTransform->GetForward().z };
	turrForward.Normalize();

	float angle = acos(turrForward.Dot(lookAt));

	Quat qRotPlus({ 0,1,0 }, angle);
	Quat qRotMinus({ 0,1,0 }, -angle);

	angle *= RADTODEG;
	DEBUG_LOG("Angle: %f", angle);


	if (angle > 1)
	{
		Quat qResultRot = turretTransform->GetRotation();

		turretTransform->SetLocalTransformation(turretTransform->GetPosition(),					// Turret Position
			qResultRot * qRotPlus,																// Turret Rotation
			turretTransform->GetScale());														// Turret Scale

		turrForward = { turretTransform->GetForward().x,0, turretTransform->GetForward().z };
		turrForward.Normalize();

		angle = acos(turrForward.Dot(lookAt));

		angle *= RADTODEG;
		DEBUG_LOG("Angle2: %f", angle);

		if (angle > 0.5 || (angle < 0.5 && angle > 0))
		{
			turretTransform->SetLocalTransformation(turretTransform->GetPosition(),
				qResultRot * qRotMinus,
				turretTransform->GetScale());
		}

	}


	if (Input::GetMouseButton(SDL_BUTTON_LEFT) == INPUT_DOWN)
	{
		if (bulletToShoot != nullptr)
		{
			GameObject* newBullet = bulletToShoot->Clone();
			newBullet->SetActive(true);
			C_Transform* bulletTrans = newBullet->GetComponent<C_Transform>();

			float3 startPos = transform->GetPosition();
			startPos.y += 5;

			bulletTrans->SetLocalTransformation(startPos, bulletTrans->GetRotation(), bulletTrans->GetScale());

			C_Script* bulletScript = newBullet->GetComponent<C_Script>();

			if (bulletScript != nullptr)
			{
				CollObject* data = bulletScript->GetObjectData();

				if (data != nullptr)
				{
					Bullet* myBullet = (Bullet*)data;

					myBullet->SetDir(turretTransform->GetForward());
				}
			}
		}

	}

}


Quat Tank::Rotation(float3 initialRotation, float3 rotation)
{
	Quat qObjectInitialRot = transform->GetRotation();

	float3 rotIncrement = rotation - initialRotation;
	rotIncrement *= DEGTORAD;

	Quat qRotIncrement = qRotIncrement.FromEulerXYZ(rotIncrement.x, rotIncrement.y, rotIncrement.z);

	return qObjectInitialRot = qObjectInitialRot * qRotIncrement;
}
