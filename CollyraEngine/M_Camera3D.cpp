#include "Application.h"
#include "M_Camera3D.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Scene.h"
#include "M_UIManager.h"

#include "GameObject.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "C_Camera.h"

#include "WG_Scene.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Math/float2.h"


M_Camera3D::M_Camera3D(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), spdMultiplier(2.0f), focusingObject(true), sceneCamera(nullptr), inputModule(nullptr)
{
	Reference = float3(0.0f, 0.0f, 0.0f);
	orbitalReference = float3(0.0f, 0.0f, -15.0f);

	sceneCamera = new C_Camera();
}

M_Camera3D::~M_Camera3D()
{}

// -----------------------------------------------------------------
bool M_Camera3D::Start()
{
	bool ret = true;

	LOG("Setting up the camera");
	inputModule = (M_Input*)App->GetModulePointer(M_INPUT);

	return ret;
}

// -----------------------------------------------------------------
bool M_Camera3D::CleanUp()
{
	LOG("Cleaning Scene Camera");

	//RELEASE(sceneCamera);
	inputModule = nullptr;

	return true;
}

// -----------------------------------------------------------------
updateStatus M_Camera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant! 

	if (inputModule == nullptr)
	{
		return UPDATE_STOP;
	}

	CameraMovement(dt);

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		FocusGameObject(App->scene->focusedGameObject);
	}

	if (inputModule->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{	
		ShootRay({ App->uiManager->GetImGuiMousePosition().x, App->uiManager->GetImGuiMousePosition().y });	
	}


	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
void M_Camera3D::CameraMovement(float dt)
{

	
	float3 newPos(0, 0, 0);
	float speed = DEFAULT_MOUSE_SPEED * dt;

	//Acceleration-------
	if (inputModule->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = (speed * 2);

	float3 zoom = CameraZoom(dt);

	if (GetMouseSceneFocus())
	{
		CameraPlanePan(newPos);

		if (inputModule->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			OrbitArroundReference(Reference, newPos, speed);
			this->orbitalReference = sceneCamera->frustum.Pos();
			this->orbitalReference += sceneCamera->frustum.Front() * 20;
		}

		else if (inputModule->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && inputModule->GetKey(SDL_SCANCODE_LALT))
		{
			OrbitArroundReference(orbitalReference, newPos, speed);
			this->Reference = sceneCamera->frustum.Pos();
			this->Reference += sceneCamera->frustum.Front() * 5;
		}
	}

	//If we are rotating arround a GameObject, we only change the focus when we move
	// This allows free zoom arround the focused GameObject
	if ((newPos.x > 0 || newPos.y > 0 || newPos.z > 0.f) && focusingObject == true)
	{
		ResetReference();
		focusingObject = false;
	}

	Move(newPos);
	Move(zoom, !focusingObject);
}

// -----------------------------------------------------------------
void M_Camera3D::CameraPlanePan(float3& newPos)
{
	if (inputModule->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = inputModule->GetMouseXMotion();
		int dy = inputModule->GetMouseYMotion();

		float Sensitivity = 0.15f;

		newPos += sceneCamera->frustum.Up() * dy * Sensitivity;
		newPos -= sceneCamera->frustum.WorldRight() * dx * Sensitivity;
	}
}

void M_Camera3D::OrbitArroundReference(float3& reference, float3& pos, float speed)
{
	int dx = -inputModule->GetMouseXMotion();
	int dy = -inputModule->GetMouseYMotion();

	float Sensitivity = 0.0025f;

	float3 vec = float3(sceneCamera->frustum.Pos() - reference);

	if (dx != 0)
	{
		float DeltaX = (float)dx * Sensitivity;

		Quat rotY(float3(0, 1, 0), DeltaX);
		rotY.Normalize();

		sceneCamera->frustum.SetUpAndFront(rotY.Transform(sceneCamera->frustum.Up()), rotY.Transform(sceneCamera->frustum.Front()));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * Sensitivity;

		Quat rotX(sceneCamera->frustum.WorldRight(), DeltaY);
		rotX.Normalize();

		sceneCamera->frustum.SetUpAndFront(rotX.Transform(sceneCamera->frustum.Up()), rotX.Transform(sceneCamera->frustum.Front()));

		if (sceneCamera->frustum.Up().y < 0.0f)
		{
			sceneCamera->frustum.SetFront(float3(0.0f, sceneCamera->frustum.Front().y > 0.0f ? 1.0f : -1.0f, 0.0f));
			sceneCamera->frustum.SetUp(float3(-sceneCamera->frustum.Front()).Cross(sceneCamera->frustum.WorldRight()));
		}
	}

	if (inputModule->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) pos += sceneCamera->frustum.Front() * speed;
	if (inputModule->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) pos -= sceneCamera->frustum.Front() * speed;

	if (inputModule->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)pos -= sceneCamera->frustum.WorldRight() * speed;
	if (inputModule->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) pos += sceneCamera->frustum.WorldRight() * speed;

	//sceneCamera->frustum.SetPos(reference + vec);
	sceneCamera->frustum.SetPos(reference - sceneCamera->frustum.Front() * vec.Length());
}

// -----------------------------------------------------------------
float3 M_Camera3D::CameraZoom(float dt)
{
	float3 zoom(0, 0, 0);

	int wheelInput = App->input->GetMouseZ();

	if (wheelInput > 0)
		zoom += sceneCamera->frustum.Front() * DEFAULT_WHEEL_SPEED * dt;
	else if (wheelInput < 0)
		zoom -= sceneCamera->frustum.Front() * DEFAULT_WHEEL_SPEED * dt;

	return zoom;
}


// -----------------------------------------------------------------
void M_Camera3D::ResetReference()
{
	this->Reference = sceneCamera->frustum.Pos();
	this->Reference += sceneCamera->frustum.Front() * 5;

	this->orbitalReference = sceneCamera->frustum.Pos();
	this->orbitalReference += sceneCamera->frustum.Front() * 20;
}

// -----------------------------------------------------------------
void M_Camera3D::Look(const float3& Position, const float3& Reference, bool RotateAroundReference)
{
	sceneCamera->frustum.SetPos(Position);
	this->Reference = Reference;

	float3 X = sceneCamera->frustum.WorldRight();
	sceneCamera->frustum.SetFront((Position - Reference).Normalized());
	X = float3(0.0f, 1.0f, 0.0f).Cross(sceneCamera->frustum.Front()).Normalized();
	sceneCamera->frustum.SetUp(sceneCamera->frustum.Front().Cross(X));

	if (!RotateAroundReference)
	{
		this->Reference = sceneCamera->frustum.Pos();
		sceneCamera->frustum.SetPos(sceneCamera->frustum.Pos() + sceneCamera->frustum.Front() * 0.05f);
	}

	this->orbitalReference = sceneCamera->frustum.Pos();
	this->orbitalReference += sceneCamera->frustum.Front() * 20;

}

void M_Camera3D::OrbitalLook(const float3& Position, const float3& Reference, bool RotateAroundReference)
{
	float3 X = sceneCamera->frustum.WorldRight();
	sceneCamera->frustum.SetFront((Reference - Position).Normalized());
	X = float3(0.0f, 1.0f, 0.0f).Cross(sceneCamera->frustum.Front()).Normalized();
	sceneCamera->frustum.SetUp(sceneCamera->frustum.Front().Cross(X));

	this->orbitalReference = Reference;
	sceneCamera->frustum.SetPos(orbitalReference - Position.Length() * sceneCamera->frustum.Front());

	if (!RotateAroundReference)
	{
		this->orbitalReference = sceneCamera->frustum.Pos();
		sceneCamera->frustum.SetPos(sceneCamera->frustum.Pos() + sceneCamera->frustum.Front() * 0.05f);
	}
	else
	{
		focusingObject = true;
	}

	this->Reference = sceneCamera->frustum.Pos();
	this->Reference += sceneCamera->frustum.Front() * 5;

}

// -----------------------------------------------------------------
void M_Camera3D::LookAt(const float3& Spot)
{
	Reference = Spot;

	float3 X = sceneCamera->frustum.WorldRight();
	sceneCamera->frustum.SetFront((sceneCamera->frustum.Pos() - Reference).Normalized());
	X = float3(0.0f, 1.0f, 0.0f).Cross(sceneCamera->frustum.Front()).Normalized();
	sceneCamera->frustum.SetUp(sceneCamera->frustum.Front().Cross(X));

}

// -----------------------------------------------------------------
void M_Camera3D::Move(const float3& Movement, bool changeReference)
{
	sceneCamera->frustum.SetPos(sceneCamera->frustum.Pos() + Movement);
	Reference += Movement;

	if (changeReference == true)
	{
		orbitalReference += Movement;
	}

}

// -----------------------------------------------------------------
void M_Camera3D::FocusGameObject(GameObject* focused, float multiplier)
{
	if (focused != nullptr)
	{
		float3 pos = { 0.0f, 0.0f, 0.0f };
		float dist = float3{ 10.0f, 10.0f, 10.0f }.Length();

		C_Transform* transform = focused->GetComponent<C_Transform>();
		float3 transformPos = transform->GetGlobalTransform().Col3(3);
		pos = { transformPos.x, transformPos.y, transformPos.z };

		float3 sizeAABB = focused->GetGameObjectAABB().Size();
		float newDist = float3{ sizeAABB.x, sizeAABB.y, sizeAABB.z }.Length();
		newDist > dist ? dist = newDist : dist;

		float3 Direction = { 1.0f, 1.0f, 1.0f };
		float3 unitDirection = (Direction).Normalized();

		float3 newPos = { 1.0f, 1.0f, 1.0f };;
		if (dist > 0.0f)
			newPos = pos + unitDirection * dist;

		OrbitalLook(newPos * multiplier, pos, true);
	}
}

// -----------------------------------------------------------------
float* M_Camera3D::GetViewMatrix()
{
	static float4x4 m;

	m = sceneCamera->frustum.ViewMatrix();

	m.Transpose();

	return (float*)&m;
}

float* M_Camera3D::GetProjectionMatrix()
{
	static float4x4 m;

	m = sceneCamera->frustum.ProjectionMatrix();

	m.Transpose();

	return (float*)&m;
}

// -----------------------------------------------------------------
float3 M_Camera3D::GetCameraPosition()
{
	return sceneCamera->frustum.Pos();
}

void M_Camera3D::SetAspectRatio(float newAspect)
{
	float horizontalFov = sceneCamera->frustum.HorizontalFov();
	sceneCamera->frustum.SetHorizontalFovAndAspectRatio(horizontalFov, newAspect);
}

// -----------------------------------------------------------------
void M_Camera3D::ShootRay(float2 mousePosition)
{

	float2 normalized;
	normalized.x = (mousePosition.x - App->uiManager->GetSceneWindowPosition().x) 
					/ (App->uiManager->GetSceneWindowSize().x * 0.5) - 1.0f;

	normalized.y = (mousePosition.y - App->uiManager->GetSceneWindowPosition().y) 
					/ (App->uiManager->GetSceneWindowSize().y * 0.5) - 1.0f;


	// Check if the mouse is inside scene window.
	if (Abs(normalized.x) < 1.0f && Abs(normalized.y) < 1.0f)
	{		
		ray = sceneCamera->frustum.UnProjectLineSegment(normalized.x, -normalized.y);

		if (!App->uiManager->sceneWindow->usingGizmo)
			App->scene->OnClickFocusGameObject(ray);
	}		
}

C_Camera* M_Camera3D::GetCamera() const
{
	return sceneCamera;
}

bool M_Camera3D::GetSceneCameraCuling() const
{
	return sceneCamera->IsCulling();
}

void M_Camera3D::SetCameraSceneCulling(bool newCullingState)
{
	sceneCamera->SetCulling(newCullingState);

}

bool M_Camera3D::GetMouseSceneFocus()
{


	float2 normalized;
	normalized.x = (App->uiManager->GetImGuiMousePosition().x - App->uiManager->GetSceneWindowPosition().x)
		/ (App->uiManager->GetSceneWindowSize().x * 0.5) - 1.0f;

	normalized.y = (App->uiManager->GetImGuiMousePosition().y - App->uiManager->GetSceneWindowPosition().y)
		/ (App->uiManager->GetSceneWindowSize().y * 0.5) - 1.0f;

	if (Abs(normalized.x) < 1.0f && Abs(normalized.y) < 1.0f)
	{
		return true;
	}
	else
		return false;
	

}

