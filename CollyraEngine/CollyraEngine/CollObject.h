#pragma once

class __declspec(dllexport) CollObject
{
public:
	CollObject();
	virtual ~CollObject();

	virtual void Start() {}

	virtual void Update() {}
};