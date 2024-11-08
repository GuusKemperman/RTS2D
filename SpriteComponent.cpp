#include "precomp.h"
#include "MySprite.h"
#include "PlayerData.h"
#include "SpriteComponent.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "Camera.h"

SpriteComponent::SpriteComponent(Entity* owner, const string& type) : 
	Component(owner, ComponentType::sprite),
	Drawable(owner->scene)
{
	PlayerData data("component.sprite." + type);
	sprite = make_unique<MySprite>(type);
	
	SetPriority(stoi(data.GetVariableValue("priority")));
}

SpriteComponent::SpriteComponent(Entity* owner, const string& type, int priority) :
	Component(owner, ComponentType::sprite),
	Drawable(owner->scene)
{
	PlayerData data("component.sprite." + type);
	sprite = make_unique<MySprite>(type);

	SetPriority(priority);
}

bool SpriteComponent::IsColliding(const SpriteComponent* other) const
{
	int2 myScrnPos = GetOwner()->GetComponent<TransformComponent>()->GetVirtualScreenPos();
	int2 otherScrnPos = other->GetOwner()->GetComponent<TransformComponent>()->GetVirtualScreenPos();

	// You don't need to draw them to a target the size of a screen, just a target that is big enough to draw both sprites.
	uint2 mySize = GetSize();
	uint2 otherSize = other->GetSize();

	int2 topLeft(min(myScrnPos.x, otherScrnPos.x), min(myScrnPos.y, otherScrnPos.y));
	int2 bottomRight(max(myScrnPos.x, otherScrnPos.x) + max(mySize.x, otherSize.x), max(myScrnPos.y, otherScrnPos.y) + max(mySize.y, otherSize.y));

	// The position of the sprite relative to the topleft of the target it will be drawn on.
	uint2 myLocalPos = make_uint2(myScrnPos - topLeft);
	uint2 otherLocalPos = make_uint2(otherScrnPos - topLeft);

	uint2 dstSize(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
	size_t vecSize = dstSize.x * dstSize.y;
	vector<bool> dst(vecSize, false);

	sprite->Draw(dst, dstSize.x, myLocalPos);

	return other->sprite->Draw(dst, dstSize.x, otherLocalPos);
}

void SpriteComponent::Draw(Surface* target, const Camera* camera) const
{
	Entity* owner = GetOwner();


	float2 worldPos = owner->GetComponent<TransformComponent>()->GetWorldPos();
	BoundingBox spriteBox = BoundingBox{ worldPos, make_float2(sprite->GetFrameSize()) };
	const BoundingBox& viewport = camera->GetViewport();

	if (viewport.Intersects(spriteBox))
	{
		int2 screenPos = camera->WorldToVirtualScreen(worldPos);
		sprite->Draw(target, screenPos);
	}
}

MySprite* SpriteComponent::GetSprite()
{
	return sprite.get();
}

uint SpriteComponent::GetWidth() const
{
	return sprite->GetFrameWidth();
}

uint SpriteComponent::GetHeight() const
{
	return sprite->GetFrameHeight();
}

uint2 SpriteComponent::GetSize() const
{
	return sprite->GetFrameSize();
}

uint SpriteComponent::GetRotationOffset(float rotation, uint numberOfMovementAngles)
{
	float anglesAsFloat = static_cast<float>(numberOfMovementAngles);
	uint offset =  static_cast<uint>(((rotation + (360.0f / (2.0f * anglesAsFloat))) * anglesAsFloat) / 360.0f);

	if (offset == numberOfMovementAngles)
	{
		offset = 0;
	}

	return offset;
}
