#include "precomp.h"
#include "Explosion.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "PlayerData.h"
#include "TimeManager.h"
#include "EntityManager.h"
#include "MySprite.h"

Explosion::Explosion(Scene& scene, const string& type, float2 atPosition) :
	OneOff(scene, 0.0f)
{
	explosionData = Explosion::Data::GetData(type);

	lifeSpan = explosionData->lifeSpan;

	float2 size = make_float2(AddComponent<SpriteComponent>(explosionData->spriteType).GetSize());
	AddComponent<TransformComponent>(atPosition - size / 2.0f);
}

Explosion::~Explosion()
{

}

void Explosion::Tick()
{
	MySprite* sprite = GetComponent<SpriteComponent>()->GetSprite();
	uint frame = static_cast<uint>((timeAlive / lifeSpan) * sprite->GetNumberOfFrames());
	sprite->SetFrame(frame);

	OneOff::Tick();
}


Explosion::Data::Data(const string& type)
{
	PlayerData playerData("entity.explosion." + type);

	lifeSpan = stof(playerData.GetVariableValue("lifetime"));
	spriteType = playerData.GetVariableValue("spritetype");
}