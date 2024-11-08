#pragma once
#include "enums.h"
#include <typeindex>

class Component;
class Scene;

class Entity
{
public:
	Entity(Scene& scene);

	virtual ~Entity();

	virtual void Tick() {};

	template<typename T, typename ... Args>
	T& AddComponent(Args&& ... args);

	template<typename T>
	inline T* GetComponent() const;

	inline const EntityId& GetId() const { return id; }

	Scene& scene;

protected:
	unordered_map<type_index, unique_ptr<Component>> components{};

private:
	EntityId id;
};

template<typename T, typename ...Args>
inline T& Entity::AddComponent(Args && ...args)
{
	unique_ptr<Component> newComponent = make_unique<T>(this, forward<Args>(args)...);
	type_index index = type_index(typeid(T));

	assert(components.find(index) == components.end());

	auto it = components.insert({ index, move(newComponent) });
	return *static_cast<T*>(it.first->second.get());
}

template<typename T>
inline T* Entity::GetComponent() const
{
	auto it = components.find(type_index(typeid(T)));

	if (it == components.end())
	{
		return nullptr;
	}
	return dynamic_cast<T*>(it->second.get());
}


