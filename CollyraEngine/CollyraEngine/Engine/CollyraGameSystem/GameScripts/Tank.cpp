#include "Tank.h"

Tank::Tank() : CollObject(), velocity(0.0f)
{
}

Tank::~Tank()
{
}

void Tank::Start()
{
	transform = GetMyGameObject()->GetComponent<C_Transform>();

	position = transform->GetPosition();
	rotation = transform->GetRotation();
	
}

void Tank::Update()
{
	PlayerInputs();
}

void Tank::PlayerInputs()
{

	// Forward: l'eix X local pero traduit al global, no. sssssss. no si clar, cla cla cla. 
	float3 forward(0.0f, 0.0f, 0.0f);
	rotation = transform->GetRotation();
	
	rotation.x *= DEGTORAD;
	rotation.y *= DEGTORAD;

	forward = { cosf(rotation.x) * cosf(rotation.y), cosf(rotation.x) * sinf(rotation.y), sinf(rotation.x) };


	if (Input::GetKey(SDL_SCANCODE_W) == INPUT_REPEAT)
	{
		velocity = 1;
	}
	else if (Input::GetKey(SDL_SCANCODE_S) == INPUT_REPEAT)
	{
		velocity = -1;
	}
	else
		velocity = 0;

	if (Input::GetKey(SDL_SCANCODE_A) == INPUT_REPEAT)
	{	
		//rotation.y += 1;
	}
	else if (Input::GetKey(SDL_SCANCODE_D) == INPUT_REPEAT)
	{
		//rotation.y -= 1;
	}



	transform->SetLocalTransformation(transform->GetPosition() + forward * velocity, transform->GetRotation(), transform->GetScale());


}
