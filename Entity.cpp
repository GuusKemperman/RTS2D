#include "precomp.h"
#include "Entity.h"
#include "Component.h"
#include "Scene.h"

Entity::Entity(Scene& a_scene) :
	scene(a_scene)
{
	static size_t numberOfEntitiesCreated{};
	id = ++numberOfEntitiesCreated;
}

Entity::~Entity()
{
	
}

