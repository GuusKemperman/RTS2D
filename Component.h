#pragma once
#include "enums.h"

class Entity;

class Component
{
public:
	Component(Entity* owner, ComponentType type);
	virtual ~Component() { owner = nullptr; };

	Component(const Component& other) = delete;
	Component& operator=(const Component& other) = delete;

	const ComponentType type;

	inline Entity* GetOwner() const { return owner; }

private:
	friend class Entity;
	Entity* owner{};
};