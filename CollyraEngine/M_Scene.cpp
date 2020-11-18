#include "M_Scene.h"
#include "GameObject.h"
#include "p2Defs.h"
#include <stack>

#include "Component.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Material.h"
#include "C_Camera.h"

#include "Application.h"
#include "M_UIManager.h"

#include "OpenGL.h"

M_Scene::M_Scene(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), root(nullptr), focusedGameObject(nullptr), sceneName("")
{}

M_Scene::~M_Scene()
{
	RELEASE(root);
}

bool M_Scene::Awake()
{
	sceneName = DEFAULT_SCENE_NAME;
	root = new GameObject(sceneName.c_str());
	root->CreateComponent(COMPONENT_TYPE::TRANSFORM);
	root->SetUid(0);

	return true;
}

updateStatus M_Scene::Update(float dt)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(root);

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

	if (root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(root);

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


		for (int i = 0; i < cameras.size(); i++)
		{
			if (currNode->GetComponent<C_Mesh>() != nullptr && cameras[i]->frustum.ContainsAABBVertices(currNode->GetGameObjectAABB()))
				currNode->GetComponent<C_Mesh>()->SetActive(true);
			else if (currNode->GetComponent<C_Mesh>() != nullptr)
				currNode->GetComponent<C_Mesh>()->SetActive(false);
		}
	}

	return UPDATE_CONTINUE;
}

updateStatus M_Scene::Draw(bool* drawState)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(root);

	while (!stack.empty())
	{
		currNode = stack.top();
		stack.pop();

		if (currNode != nullptr && currNode->active)
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

bool M_Scene::CleanUp()
{
	RELEASE(root);
	return true;
}

GameObject* M_Scene::CreateGameObject(std::string name, GameObject* parent)
{
	if (parent == nullptr)
		parent = root;

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
	sceneName = DEFAULT_SCENE_NAME;
	root->SetName(DEFAULT_SCENE_NAME);

	for (uint i = 0; i < root->children.size(); i++)
	{
		App->uiManager->GameObjectDestroyed(root->children[i]->GetUid());
		RELEASE(root->children[i]);
	}
	root->children.clear();
}

uint32 M_Scene::GenerateId()
{
	return randomGenerator.Int();
}

GameObject* M_Scene::GetRoot()
{
	return root;
}

std::string M_Scene::GetSceneName() const
{
	return sceneName;
}

void M_Scene::SetSceneName(const char* newName)
{
	sceneName = newName;
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

GameObject* M_Scene::GetGameObject(unsigned int id)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (root == nullptr)
	{
		LOG("Root node did not exist!");
		return nullptr;
	}

	stack.push(root);

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

GameObject* M_Scene::GetGameObject(std::string name)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;

	if (root == nullptr)
	{
		LOG("Root node did not exist!");
		return nullptr;
	}

	stack.push(root);

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


bool M_Scene::DeleteGameObject(unsigned int id)
{
	std::stack<GameObject*> stack;
	GameObject* currNode = nullptr;
	GameObject* toDelete = nullptr;

	if (root == nullptr)
	{
		LOG("Root node did not exist!");
		return UPDATE_STOP;
	}

	stack.push(root);

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
