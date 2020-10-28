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
	void SetColor(Color color);
	void SetDefaultTexture();

	uint GetTexture() const;
	Color GetColor() const;

private:

	void ClearTexture();

private:

	uint idTextureImage;
	Color color;

};