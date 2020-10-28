#include "M_Scene.h"
#include "GameObject.h"
#include "p2Defs.h"

#include "Component.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Material.h"

#include "Application.h"
#include "M_UIManager.h"

M_Scene::M_Scene(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), root(nullptr), globalId(0)
{}

M_Scene::~M_Scene()
{}

bool M_Scene::Awake()
{
	root = new GameObject("Scene");
	root->SetId(GenerateId());

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

updateStatus M_Scene::Draw(float dt)
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
			DrawGameObject(currNode);
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

updateStatus M_Scene::DebugDraw(float dt)
{
	return updateStatus();
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
	newGameObject->parent = parent;
	newGameObject->SetId(GenerateId());

	newGameObject->CreateComponent(COMPONENT_TYPE::TRANSFORM);

	return newGameObject;
}

uint M_Scene::GenerateId()
{
	return globalId++;
}

const GameObject* M_Scene::GetRoot()
{
	return root;
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

void M_Scene::DrawGameObject(GameObject* gameObject)
{
	C_Mesh* mesh = gameObject->GetComponent<C_Mesh>();
	C_Transform* transform = gameObject->GetComponent<C_Transform>();
	C_Material* material = gameObject->GetComponent<C_Material>();

	if (mesh != nullptr && transform != nullptr)
	{
		if (material != nullptr)
		{
			mesh->Render(transform->GetTGlobalTransform(), (int)material->GetTexture(), material->GetColor());
		}
		else
		{
			mesh->Render(transform->GetTGlobalTransform());
		}
	}
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

		if (currNode->GetId() == id)
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

		if (currNode->GetId() == id)
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
	App->uiManager->GameObjectDestroyed(gameObject->GetId());

	//Notify the Parent that his child is going to die-----------
	if (gameObject->parent != nullptr)
		gameObject->parent->NotifyChildDeath(gameObject);

	RELEASE(gameObject);

	return true;
}