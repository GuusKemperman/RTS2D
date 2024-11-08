#include "precomp.h"
#include "OneOff.h"
#include "TimeManager.h"
#include "EntityManager.h"
#include "Scene.h"

OneOff::OneOff(Scene& scene, float a_lifeSpan) :
	Entity(scene)
{
	lifeSpan = a_lifeSpan;
}

OneOff::~OneOff()
{
}

void OneOff::Tick()
{
	timeAlive += TimeManager::GetDeltaTime();

	if (timeAlive >= lifeSpan)
	{
		scene.entityManager->RemoveEntity(GetId());
		LifeSpanReached();
		return;
	}
}
