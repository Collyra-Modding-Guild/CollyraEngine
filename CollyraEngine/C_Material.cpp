#include "C_Material.h"
#include "TextureLoader.h"

#include "Application.h"
#include "M_Resources.h"

#include "OpenGL.h"

C_Material::C_Material(bool active) : Component(COMPONENT_TYPE::MATERIAL, active), idTextureImage(0), color(Grey), useDefaultTexture(false),
defaultTextureId(App->resources->defaultTextureId)
{}

C_Material::~C_Material()
{
	ClearTexture();
}

void C_Material::SetTexture(uint idTexture)
{
	ClearTexture();

	idTextureImage = idTexture;
}

void C_Material::SetTextureNameAndPath(const char* name, const char* path)
{
	texName = name;
	texPath = path;
}

void C_Material::SetColor(Color color)
{
	this->color = color;
}

void C_Material::SetUseDefaultTexture(bool defaultTextureOn)
{
	if (defaultTextureOn != useDefaultTexture)
		useDefaultTexture = !useDefaultTexture;
}

bool C_Material::GetDefaultTextureUsage() const
{
	return useDefaultTexture;
}

uint C_Material::GetTexture() const
{
	if (useDefaultTexture == true)
		return defaultTextureId;

	return idTextureImage;
}

Color C_Material::GetColor() const
{
	return color;
}

void C_Material::ClearTexture()
{
	glDeleteTextures(1, &idTextureImage);
}
