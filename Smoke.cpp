#include "precomp.h"
#include "Smoke.h"
#include "TransformComponent.h"
#include "TimeManager.h"
#include "DrawFuncs.h"
#include "Camera.h"

Smoke::Smoke(Scene& scene, float2 position, float lifeSpan, float a_maxRadius) :
	Drawable(scene, 100),
	OneOff(scene, lifeSpan),
	maxRadius(a_maxRadius)
{
	AddComponent<TransformComponent>().SetWorldPos(position);
}

void Smoke::Tick()
{
	float deltaTime = TimeManager::GetDeltaTime();

	acceleration += (float2(Rand(1.0f) - .5f, Rand(1.0f) - .5f) * accelerationFactor + wind) * deltaTime;
	acceleration *= powf(friction, deltaTime);
	velocity += acceleration * deltaTime;

	GetComponent<TransformComponent>()->TranslateWorldPos(velocity * deltaTime);

	OneOff::Tick();
}

void Smoke::Draw(Surface* target, const Camera* camera) const
{
	int2 screenPos = GetComponent<TransformComponent>()->GetVirtualScreenPos();

	float lifePercentage = clamp((timeAlive / lifeSpan), 0.0f, 1.0f);
	float  radius = lifePercentage * maxRadius;

	uint alpha = static_cast<uint>((1.0f - lifePercentage) * maxAlpha);

	DrawFuncs::DrawCircle(target, screenPos, static_cast<int>(radius), (alpha << 24) + smokeCol);
}
