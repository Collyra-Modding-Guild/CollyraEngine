#include "M_Scene.h"
#include "GameObject.h"
#include "p2Defs.h"
#include <stack>
#include <map>

#include "Component.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Material.h"
#include "C_Camera.h"
#include "C_Script.h"

#include "Application.h"
#include "M_Camera3D.h"
#include "M_UIManager.h"
#include "M_Renderer3D.h"
#include "SceneLoader.h"
#include "M_FileManager.h"
#include "M_Resources.h"
#include "Timer.h"

#include "R_Resource.h"
#include "R_Scene.h"
#include "R_Material.h"

#include "OpenGL.h"

M_Scene::M_Scene(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), focusedGameObject(nullptr), currentScene(nullptr),
playedScene(0), savedScenePath()
{}

M_Scene::~M_Scene()
{}

bool M_Scene::Awake()
{
	randomGenerator = LCG::LCG();

	currentScene = (R_Scene*)App->resources->CreateResource(R_TYPE::SCENE);
	currentScene->referenceCount++;

	savedScenePath = currentScene->GetLibraryPath();

	currentScene->root = new GameObject(DEFAULT_SCENE_NAME);
	currentScene->root->CreateComponent(COMPONENT_TYPE::TRANSFORM);
	currentScene->root->SetUid(0);
	SetSceneName(DEFAULT_SCENE_NAME);

	GameObject* camera = App->scene->CreateGameObject("Camera");
	camera->CreateComponent(COMPONENT_TYPE::CAMERA);

	return true;
}

bool M_Scene::Start()
{
	uint defaultSceneId = App->resources->LoadDefaultScene();
	if (defaultSceneId != 0)
	{
		App->resources->RequestResource(defaultSceneId);
	}
	return true;
}

updateStatus M_Scene::Update(float dt)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode != nullptr)
		{
			currNode->Update(dt);
		}
		else
			continue;

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

	return UPDATE_CONTINUE;
}

updateStatus M_Scene::PostUpdate(float dt)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode != nullptr)
		{
			currNode->PostUpdate(dt);
		}
		else
			continue;

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}

		int checkCulling = 0;

		for (int i = 0; i < cameras.size(); i++)
		{
			C_Camera* currentCam = cameras[i];

			if (CameraCuling(currNode, cameras[i]))
				checkCulling++;
		}
		if (App->camera->GetCamera())
		{
			if (CameraCuling(currNode, App->camera->GetCamera()))
				checkCulling++;
		}

		if (currNode->GetComponent<C_Mesh>() != nullptr)
		{
			if (checkCulling > 0)
			{
				currNode->GetComponent<C_Mesh>()->SetActive(true);
			}
			else
			{
				currNode->GetComponent<C_Mesh>()->SetActive(false);
			}
		}


	}

	return UPDATE_CONTINUE;
}

updateStatus M_Scene::Draw(bool* drawState)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode != nullptr && currNode->IsActive())
		{
			DrawGameObject(currNode, drawState);
		}
		else
			continue;

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}
}

updateStatus M_Scene::PreDraw(bool* drawState)
{
	if (drawState[OUTLINE] == false)
	{
		if (focusedGameObject != nullptr)
		{
			C_Mesh* mesh = focusedGameObject->GetComponent<C_Mesh>();
			C_Transform* transform = focusedGameObject->GetComponent<C_Transform>();

			if ((mesh != nullptr && mesh->IsActive() == true) && transform != nullptr)
			{
				mesh->DrawOutline(transform->GetTGlobalTransform());
			}
		}
	}
	else
	{
		std::stack<GameObject*> stack;
		GameObject* currNode = nullptr;

		if (currentScene->root == nullptr)
		{
			LOG("Root node did not exist!");
			return UPDATE_STOP;
		}

		stack.push(currentScene->root);

		while (!stack.empty())
		{
			currNode = stack.top();
			stack.pop();

			if (currNode != nullptr && currNode->IsActive())
			{
				C_Mesh* mesh = currNode->GetComponent<C_Mesh>();
				C_Transform* transform = currNode->GetComponent<C_Transform>();

				if ((mesh != nullptr && mesh->IsActive() == true) && transform != nullptr)
				{
					mesh->DrawOutline(transform->GetTGlobalTransform());
				}
			}
			else
				continue;

			int childNum = currNode->children.size();
			for (int i = 0; i < childNum; i++)
			{
				stack.push(currNode->children[i]);
			}
		}
	}

	return UPDATE_CONTINUE;
}

bool M_Scene::CleanUp()
{
	//RELEASE(currentScene->root);
	return true;
}

GameObject* M_Scene::CreateGameObject(std::string name, GameObject* parent)
{
	if (parent == nullptr)
		parent = currentScene->root;

	if (name == "")
	{
		name = "Empty GameObject";
	}

	CheckSiblingsName(parent, name);

	GameObject* newGameObject = new GameObject(name);

	parent->children.push_back(newGameObject);
	newGameObject->SetParent(parent);
	newGameObject->SetUid(GenerateId());

	newGameObject->CreateComponent(COMPONENT_TYPE::TRANSFORM);

	return newGameObject;
}

void M_Scene::ResetScene()
{
	if (currentScene == nullptr)
		return;

	App->uiManager->SetFocusedGameObject(-1);
	focusedGameObject = nullptr;

	for (uint i = 0; i < currentScene->root->children.size(); i++)
	{
		App->uiManager->GameObjectDestroyed(currentScene->root->children[i]->GetUid());
		RELEASE(currentScene->root->children[i]);
	}

	currentScene->root->children.clear();
	currentScene->root->SetName(DEFAULT_SCENE_NAME);
}

uint M_Scene::SaveScene()
{
	App->physFS->DeleteFileIn(savedScenePath.c_str());
	savedScenePath = currentScene->GetLibraryPath();
	return App->resources->SaveResource(currentScene, "", false);
}

uint32 M_Scene::GenerateId()
{
	return randomGenerator.Int();
}

GameObject* M_Scene::GetRoot()
{
	return currentScene->root;
}

std::string M_Scene::GetSceneName() const
{
	return currentScene->root->GetName();
}

void M_Scene::SetSceneName(const char* newName)
{
	currentScene->root->SetName(newName);
	currentScene->SetName(newName);
	currentScene->SetLibraryPath(App->physFS->GetExtensionFolderLibraryFromType(R_TYPE::SCENE).append(currentScene->GetName().append(App->physFS->GetInternalExtensionFromType(R_TYPE::SCENE))));
}

R_Scene* M_Scene::GetSceneResource() const
{
	return currentScene;
}

void M_Scene::SetSceneResource(R_Scene* newScene)
{
	currentScene->SetUid(newScene->GetUid());
}

void M_Scene::ResoucesUpdated(std::map<uint, bool>* updatedId)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode != nullptr)
		{
			currNode->ResourcesUpdated(updatedId);
		}
		else
			continue;

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

	return;
}

void M_Scene::SetResourceToGameObject(uint resourceId, R_TYPE rType, GameObject* setTo)
{
	if (setTo == nullptr)
	{
		setTo = focusedGameObject;
		if (setTo == nullptr)
		{
			LOG("You need to have a GameObject selected!!!");
			return;
		}
	}

	switch (rType)
	{
	case R_TYPE::TEXTURE:
	{
		C_Material* cmp = setTo->GetComponent<C_Material>();

		if (cmp == nullptr)
		{
			cmp = (C_Material*)setTo->CreateComponent(COMPONENT_TYPE::MATERIAL);
		}

		uint myResourceId = cmp->GetResourceId();
		R_Material* myResource = nullptr;

		//No material loaded, let's create one
		if (myResourceId == 0)
		{
			myResourceId = App->resources->CreateResource(R_TYPE::MATERIAL)->GetUid();
			cmp->SetResourceId(myResourceId);
		}

		myResource = cmp->GetResourcePointer();
		myResource->SetTextureResourceId(resourceId);
	}
	break;
	case R_TYPE::MATERIAL:
	{
		C_Material* cmp = setTo->GetComponent<C_Material>();

		if (cmp == nullptr)
		{
			cmp = (C_Material*)setTo->CreateComponent(COMPONENT_TYPE::MATERIAL);
		}

		cmp->SetResourceId(resourceId);
	}
	break;
	case R_TYPE::MESH:
	{
		C_Mesh* cmp = setTo->GetComponent<C_Mesh>();

		if (cmp == nullptr)
		{
			cmp = (C_Mesh*)setTo->CreateComponent(COMPONENT_TYPE::MESH);
		}

		cmp->SetResourceId(resourceId);
	}
	break;
	default:
		break;
	}

}

void M_Scene::GenerateNewScene()
{
	if (currentScene != nullptr)
	{
		ResetScene();
		App->resources->UnloadResource(currentScene->GetUid());
	}

	currentScene->SetUid(GenerateId());
	SetSceneName(DEFAULT_SCENE_NAME);
	savedScenePath = currentScene->GetLibraryPath();
	GameObject* camera = App->scene->CreateGameObject("Camera");
	camera->CreateComponent(COMPONENT_TYPE::CAMERA);

}

void M_Scene::PrerHotReload()
{
	allScriptComponents = GetAllComponents<C_Script>();

	for (int i = 0; i < allScriptComponents.size(); i++)
	{
		//TODO: We should save the variables here, save them in the component
		allScriptComponents[i]->DeleteObjectData();
	}
}

void M_Scene::PostrHotReload()
{
	for (int i = 0; i < allScriptComponents.size(); i++)
	{
		allScriptComponents[i]->GenerateObjectData();
		//TODO: We should load our variables here, stored in the component
	}
}

void M_Scene::CheckSiblingsName(GameObject* parent, std::string& myName)
{
	uint siblingSameName = 0;
	for (int i = 0; i < parent->children.size(); i++)
	{
		std::size_t found = parent->children[i]->GetName().find(myName);
		if (found != std::string::npos)
			siblingSameName++;
	}

	if (siblingSameName > 0)
	{
		char str[10];
		sprintf_s(str, 10, "%i", siblingSameName);
		std::string number = str;
		myName += " (" + number + ")";
	}
}

void M_Scene::DrawGameObject(GameObject* gameObject, bool* drawState)
{
	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	C_Material* material = gameObject->GetComponent<C_Material>();
	C_Camera* camera = gameObject->GetComponent<C_Camera>();

	if ((mesh != nullptr && mesh->IsActive() == true) && transform != nullptr)
	{
		if (material != nullptr && material->IsActive() == true)
		{
			mesh->Render(drawState, transform->GetTGlobalTransform(), (int)material->GetTexture(), material->GetColor());
		}
		else
		{
			mesh->Render(drawState, transform->GetTGlobalTransform());
		}
	}

	if (transform != nullptr && drawState[BOUNDING_BOX])
		DrawBoundingBox(gameObject);

	if (camera != nullptr && drawState[FRUSTUM])
		DrawFrustum(gameObject);

	if (drawState[MOUSE_RAY])
		DrawMouseRay(&App->camera->ray);

}

void M_Scene::DrawBoundingBox(GameObject* gameObject)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	glLineWidth(2.0f);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	for (uint i = 0; i < gameObject->GetGameObjectAABB().NumEdges(); i++)
	{
		glVertex3f(gameObject->GetGameObjectAABB().Edge(i).a.x,
			gameObject->GetGameObjectAABB().Edge(i).a.y,
			gameObject->GetGameObjectAABB().Edge(i).a.z);

		glVertex3f(gameObject->GetGameObjectAABB().Edge(i).b.x,
			gameObject->GetGameObjectAABB().Edge(i).b.y,
			gameObject->GetGameObjectAABB().Edge(i).b.z);
	}

	glEnd();
	glEnable(GL_LIGHTING);
}

void M_Scene::DrawFrustum(GameObject* gameObject)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	glLineWidth(2.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	Frustum frustum = gameObject->GetComponent<C_Camera>()->frustum;

	for (uint i = 0; i < frustum.NumEdges(); i++)
	{
		glVertex3f(frustum.Edge(i).a.x,
			frustum.Edge(i).a.y,
			frustum.Edge(i).a.z);

		glVertex3f(frustum.Edge(i).b.x,
			frustum.Edge(i).b.y,
			frustum.Edge(i).b.z);
	}

	glEnd();
	glEnable(GL_LIGHTING);

}

void M_Scene::DrawMouseRay(LineSegment* mouseRay)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

	glLineWidth(2.0f);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	glVertex3f(mouseRay->a.x, mouseRay->a.y, mouseRay->a.z);
	glVertex3f(mouseRay->b.x, mouseRay->b.x, mouseRay->b.z);

	glEnd();
	glEnable(GL_LIGHTING);
}

GameObject* M_Scene::GetGameObject(unsigned int id)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return nullptr;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode->GetUid() == id)
		{
			return currNode;
		}

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

	return nullptr;
}

template<typename T>
std::vector<T*> M_Scene::GetAllComponents()
{
	std::vector<GameObject*> allGameObjects = GetAllGameobjects();

	std::vector<T*> ret;

	for (int i = 0; i < allGameObjects.size(); i++)
	{
		const std::vector<Component*>* currentComponents =  allGameObjects[i]->GetAllComponents();

		for (int y = 0; y < currentComponents->size(); y++)
		{
			Component* currComponent = currentComponents->at(y);
			T* c = dynamic_cast<T*>(currComponent);
			if (c != nullptr)
				ret.push_back(c);
		}
	}

	return ret;
}


GameObject* M_Scene::GetGameObject(std::string name)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return nullptr;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode->GetName() == name)
		{
			return currNode;
		}

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

	return nullptr;
}

std::vector<GameObject*> M_Scene::GetAllGameobjects()
{
	std::vector<GameObject*> allGO;
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return allGO;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		allGO.push_back(currNode);

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

	return allGO;
}


bool M_Scene::DeleteGameObject(unsigned int id)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;
	GameObject* toDelete = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode->GetUid() == id)
		{
			toDelete = currNode;
			break;
		}

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

	bool ret = false;

	if (toDelete != nullptr)
		ret = DeleteGameObject(toDelete);

	return ret;
}


bool M_Scene::DeleteGameObject(GameObject* gameObject)
{
	if (gameObject == nullptr)
		return false;

	if (focusedGameObject == gameObject)
		focusedGameObject = nullptr;

	//Notify the UI that a GameObject has been destroyed---------
	App->uiManager->GameObjectDestroyed(gameObject->GetUid());

	//Notify the Parent that his child is going to die-----------
	if (gameObject->GetParent() != nullptr)
		gameObject->GetParent()->NotifyChildDeath(gameObject);

	RELEASE(gameObject);

	return true;
}

void M_Scene::DeleteCamera(Component* component)
{
	for (int i = 0; i < cameras.size(); i++)
	{
		if (cameras[i] == component)
		{
			cameras.erase(cameras.begin() + i);
		}
	}
}

void M_Scene::StartGameObjects()
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;
	GameObject* toDelete = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		currNode->Start();

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

}

void M_Scene::OnClickFocusGameObject(const LineSegment& mouseRay)
{
	std::stack<GameObject*> stack;
	std::map<float, GameObject*> inRay;

	GameObject* currNode = nullptr;

	if (currentScene->root == nullptr)
	{
		LOG("Root node did not exist!");
		return;
	}

	stack.push(currentScene->root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		float nearHit = 0.0f;
		float farHit = 0.0f;

		if (mouseRay.Intersects(currNode->GetGameObjectAABB(), nearHit, farHit) && currNode->GetUid() != ROOT_ID)
		{
			inRay.insert({ nearHit, currNode });
		}

		int childNum = currNode->children.size();
		for (int i = 0; i < childNum; i++)
		{
			stack.push(currNode->children[i]);
		}
	}

	std::map<float, GameObject*>::iterator iterator = inRay.begin();

	for (int i = 0; i < inRay.size(); i++)
	{
		C_Mesh* mesh = iterator->second->GetComponent<C_Mesh>();

		if (mesh != nullptr)
		{

			C_Transform* transform = iterator->second->GetComponent<C_Transform>();
			LineSegment localRay = mouseRay;

			localRay.Transform(transform->GetGlobalTransform().Inverted());
			std::vector<uint>* indices = mesh->GetIndices();
			std::vector<float3>* vertices = mesh->GetVertices();

			for (uint j = 0; j < mesh->GetIndicesSize() - 2; j++)
			{
				uint indexA = indices->at(j);
				uint indexB = indices->at(j + 1);
				uint indexC = indices->at(j + 2);

				vec verticeA(vertices->at(indexA));
				vec verticeB(vertices->at(indexB));
				vec verticeC(vertices->at(indexC));

				Triangle meshTriangle(verticeA, verticeB, verticeC);

				if (localRay.Intersects(meshTriangle, nullptr, nullptr))
				{
					App->uiManager->SetFocusedGameObject(iterator->second->GetUid());
					return;
				}
			}
		}
		else
		{
			App->uiManager->SetFocusedGameObject(iterator->second->GetUid());
			return;
		}

		iterator++;
	}

	App->uiManager->SetFocusedGameObject(-1);
}

bool M_Scene::CameraCuling(GameObject* current, C_Camera* myCam)
{
	if (current->GetComponent<C_Mesh>() != nullptr && myCam->IsCulling() && myCam->frustum.ContainsAABBVertices(current->GetGameObjectAABB()))
	{
		return true;
	}
	else if (current->GetComponent<C_Mesh>() != nullptr && myCam->IsCulling())
	{
		return false;
	}
}

void M_Scene::Play()
{
	if (App->uiManager->GetPlayCam() != nullptr)
	{
		App->gameClock->Start();
		App->StartPlayMode();
		App->renderer3D->SetPlayCam(App->uiManager->GetPlayCam());
	}
	else
		LOG("A cam needs to be selected to start the scene!");

}

updateStatus M_Scene::StartPlayMode()
{
	SaveScene();
	playedScene = this->GetSceneResource()->GetUid();

	StartGameObjects();
	return UPDATE_CONTINUE;
}

void M_Scene::Stop()
{
	App->gameClock->Stop();
	App->resources->LoadResource(playedScene);
	App->EndPlayMode();
}

updateStatus M_Scene::StopPlayMode()
{
	App->renderer3D->SetPlayCam(nullptr);

	return UPDATE_CONTINUE;
}

void M_Scene::Pause()
{
	App->gameClock->Pause();
}

void M_Scene::Resume()
{
	App->gameClock->Resume();
}

void M_Scene::Tick()
{
	App->gameClock->Tick(true);
}
