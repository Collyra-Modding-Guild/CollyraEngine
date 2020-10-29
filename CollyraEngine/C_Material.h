#pragma once

#include "Component.h"
#include "Globals.h"
#include "Color.h"


class C_Material : public Component
{
public:

	C_Material(bool active = true);
	~C_Material();

	void SetTexture(uint idTexture);
	void SetTextureNameAndPath(const char* name, const char* path);
	void SetColor(Color color);
	void SetUseDefaultTexture(bool defaultTextureOn);
	bool GetDefaultTextureUsage() const;

	uint GetTexture() const;
	Color GetColor() const;

	std::string GetMaterialName() const;
	std::string GetMaterialPath() const;

private:

	void ClearTexture();

private:

	uint idTextureImage;
	Color color;
	std::string name;
	std::string path;

	bool useDefaultTexture;
	uint defaultTextureId;

};