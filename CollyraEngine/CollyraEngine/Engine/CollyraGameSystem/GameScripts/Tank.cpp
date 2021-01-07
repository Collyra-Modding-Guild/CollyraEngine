#include "Tank.h"

Tank::Tank() : CollObject(), velocity(0.0f), myTurret(nullptr)
{
}

Tank::~Tank()
{
}

void Tank::Start()
{
	transform = GetMyGameObject()->GetComponent<C_Transform>();

	myTurret = gameObject::GetGameObjectByName("TankTurret");
	//turretTransform = myTurret->GetComponent<C_Transform>();
}

void Tank::Update()
{
	PlayerInputs();
}

void Tank::PlayerInputs()
{
	float3 forward = transform->GetForward();

	if (Input::GetKey(SDL_SCANCODE_W) == INPUT_REPEAT)
	{
		velocity = 0.1f;
	}
	else if (Input::GetKey(SDL_SCANCODE_S) == INPUT_REPEAT)
	{
		velocity = -0.1f;
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

	transform->SetLocalTransformation(transform->GetPosition() + forward * velocity, // Tank Position
									  Rotation(initialRot, rotation),				 // Tank Rotation
									  transform->GetScale());						 // Tank Scale


/*	float3 turRotation = turretTransform->GetRotationEuler();
	float3 turInitialRot = rotation;



	if (Input::GetKey(SDL_SCANCODE_LEFT) == INPUT_REPEAT)
	{
		turRotation.y += 1;
	}
	else if (Input::GetKey(SDL_SCANCODE_RIGHT) == INPUT_REPEAT)
	{
		turRotation.y -= 1;
	}



	turretTransform->SetLocalTransformation(turretTransform->GetPosition(), // Turret Position
		Rotation(turInitialRot, turRotation),								// Turret Rotation
		turretTransform->GetScale());				*/						// Turret Scale
}


Quat Tank::Rotation(float3 initialRotation, float3 rotation)
{
	Quat qObjectInitialRot = transform->GetRotation();

	float3 rotIncrement = rotation - initialRotation;
	rotIncrement *= DEGTORAD;

	Quat qRotIncrement = qRotIncrement.FromEulerXYZ(rotIncrement.x, rotIncrement.y, rotIncrement.z);

	return qObjectInitialRot = qObjectInitialRot * qRotIncrement;
}
