#pragma once
#include "Drawable.h"
#include "Component.h"

class ColliderComponent;
class MySprite;

class SpriteComponent :
	public Component, 
	public Drawable
{
public:
	SpriteComponent(Entity* owner, const string& type);
	SpriteComponent(Entity* owner, const string& type, int priority);

	bool IsColliding(const SpriteComponent* other) const;

	void Draw(Surface* target, const Camera* camera) const override;
	
	MySprite* GetSprite();
	uint GetWidth() const;
	uint GetHeight() const;
	uint2 GetSize() const;

	static uint GetRotationOffset(float rotation, uint numberOfMovementAngles);

private:
	std::unique_ptr<MySprite> sprite{};
};
