#include "M_Scene.h"
#include "GameObject.h"
#include "p2Defs.h"

#include "Component.h"
#include "C_Mesh.h"
#include "C_Transform.h"
#include "C_Material.h"

M_Scene::M_Scene(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), root(nullptr), globalId(0)
{}

M_Scene::~M_Scene()
{}

bool M_Scene::Awake()
{
	root = new GameObject("root");
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

		if (currNode != nullptr)
		{
			C_Mesh* mesh = currNode->GetComponent<C_Mesh>();
			C_Transform* transform = currNode->GetComponent<C_Transform>();
			C_Material* material = currNode->GetComponent<C_Material>();
			
			if (mesh != nullptr && transform != nullptr)
			{
				if (material != nullptr) 
				{
					mesh->Render(transform->GetTGlobalTransform(), material->GetTexture(), material->GetColor());
				}
				else 
				{
					mesh->Render(transform->GetTGlobalTransform());
				}					
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

updateStatus M_Scene::DebugDraw(float dt)
{
	return updateStatus();
}

bool M_Scene::CleanUp()
{
	RELEASE(root);
	return true;
}

GameObject* M_Scene::CreateGameObject(std::string name,GameObject* parent)
{
	if (parent == nullptr)
		parent = root;

	//TODO: Make that names follow Empty(n)
	if (name == "")
	{
		name = "Empty GameObject";
	}

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

	if (toDelete != nullptr)
		DeleteGameObject(toDelete);

	return false;
}

bool M_Scene::DeleteGameObject(GameObject* gameObject)
{
	RELEASE(gameObject);

	return true;
}

