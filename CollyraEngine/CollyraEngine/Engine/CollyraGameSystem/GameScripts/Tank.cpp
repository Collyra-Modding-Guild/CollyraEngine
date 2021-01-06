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
	float3 forward(0.0f, 0.0f, 0.0f);
	rotation = transform->GetRotation();
	
	/*rotation.x *= DEGTORAD;
	rotation.y *= DEGTORAD;
	rotation.z *= DEGTORAD;*/

	forward = { 2 * (rotation.x * rotation.z + rotation.w * rotation.y),
			2 * (rotation.y * rotation.z - rotation.w * rotation.x),
			1 - 2 * (rotation.x * rotation.x + rotation.y * rotation.y) };

	DEBUG_LOG("X: %f, Y: %f, Z: %f", forward.x, forward.y, forward.z);

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
