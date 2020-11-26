#include "R_Material.h"


R_Material::R_Material(unsigned int uid) : Resource(uid, R_TYPE::MATERIAL), idTextureResource(-1)
{}


R_Material::~R_Material()
{}

void R_Material::SetColor(Color color)
{
	this->color = color;
}

void R_Material::SetNameAndPath(const char* name, const char* path)
{
	this->name = name;
	this->path = path;
}

void R_Material::SetTextureResourceId(uint rTexId)
{
	idTextureResource = rTexId;
}

std::string R_Material::GetName() const
{
	return name;
}

std::string R_Material::GetPath() const
{
	return path;
}

Color R_Material::GetColor() const
{
	return color;
}

int R_Material::GetTextureId() const
{
	return idTextureResource;
}

void R_Material::SetTextureId(int newId)
{
	idTextureResource = newId;
}


