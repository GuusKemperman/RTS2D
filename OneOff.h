#pragma once
#include "Entity.h"

// Entity that gets destroyed after a fixed amount of time.
class OneOff : 
	public Entity
{
public:
	OneOff(Scene& scene, float lifeSpan);
	virtual ~OneOff();

	virtual void Tick() override;

	virtual void LifeSpanReached() {};
protected:
	float lifeSpan{};
	float timeAlive{};
};

