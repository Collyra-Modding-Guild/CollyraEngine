#pragma once

#include "Component.h"
#include "Globals.h"
#include "Color.h"

class R_Material;

class C_Material : public Component
{
public:

	C_Material(bool active = true);
	~C_Material();

	void SetUseDefaultTexture(bool defaultTextureOn);
	bool GetDefaultTextureUsage() const;

	uint GetTexture() const;
	Color GetColor() const;
	void SetColor(Color newColor);

	std::string GetMaterialName() const;
	std::string GetMaterialPath() const;
	std::string GetTextureName() const;
	std::string GetTexturePath() const;

	void SetResourceId(uint newId);
	int GetResourceId() const;
	R_Material* GetResourcePointer() const;
	void ResourceUpdated(std::map<uint, bool>* ids);


private:

	bool useDefaultTexture;
	uint defaultTextureId;

	int resourceId;
	R_Material* myResource;

};