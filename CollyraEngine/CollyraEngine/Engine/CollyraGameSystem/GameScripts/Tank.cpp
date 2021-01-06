#include "Tank.h"

Tank::Tank() : CollObject()
{
}

Tank::~Tank()
{
}

void Tank::Start()
{
	transform = GetMyGameObject()->GetComponent<C_Transform>();
}

void Tank::Update()
{
	PlayerInputs();
}

void Tank::PlayerInputs()
{
	if (Input::GetKey(SDL_SCANCODE_W) == INPUT_REPEAT)
	{
		Debug::Log("Advance");
		//transform->SetLocalTransformation({ 1.0f, 0.0f, 0.0f }, transform->GetRotation(), transform->GetScale());
	}
	if (Input::GetKey(SDL_SCANCODE_A) == INPUT_REPEAT)
	{
		Debug::Log("Left");
	}
	if (Input::GetKey(SDL_SCANCODE_D) == INPUT_REPEAT)
	{
		Debug::Log("Right");
	}
	if (Input::GetKey(SDL_SCANCODE_S) == INPUT_REPEAT)
	{
		Debug::Log("Backwards");
	}

}
