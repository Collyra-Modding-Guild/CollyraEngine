#include "M_Scene.h"

M_Scene::M_Scene(MODULE_TYPE type, bool startEnabled) : Module(type, startEnabled), root(nullptr)
{
}

M_Scene::~M_Scene()
{
}

bool M_Scene::Awake()
{
    return true;
}

updateStatus M_Scene::Update(float dt)
{
    return UPDATE_CONTINUE;
}

bool M_Scene::CleanUp()
{
    return true;
}
