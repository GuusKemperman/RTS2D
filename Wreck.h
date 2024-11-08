#pragma once
#include "OneOff.h"
#include "Military.h"

class Wreck : 
	public OneOff
{
public:
	Wreck(Scene& scene, const Military* military, float2 position, float rotation);
	~Wreck();

	void Tick() override;

	void LifeSpanReached() override;

private:
	const Military::Data* militaryData{};
	
	static constexpr float wreckLifeSpan = 15.0f;
	static constexpr float smokeCooldown = 2.5f;
	float timeSinceSmoke = smokeCooldown;
};