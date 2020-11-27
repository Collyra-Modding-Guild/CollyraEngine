#include "R_Scene.h"


R_Scene::R_Scene(unsigned int uid) : Resource(uid, R_TYPE::SCENE), root(nullptr)
{}


R_Scene::~R_Scene()
{}