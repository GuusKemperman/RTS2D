#pragma once
#include "Component.h"
#include "Observer.h"
#include "BoundingBox.h"
#include "NearbyCollider.h"

class TransformComponent;


class ColliderComponent :
	public Component, 
	public Observer
{
public:
	ColliderComponent(Entity* owner, CollisionLayer layer = 0xff);
	~ColliderComponent();

	// When the entity has moved
	void ObservationMade();

	inline const BoundingBox& GetBoundingBox() const { return worldBox; }
	inline float2 GetCentre() const { return worldBox.GetCentre(); }

	vector<ColliderComponent*> CollisionsWhenAt(float2 worldPos);
	vector<ColliderComponent*> Collisions();

	// Returns vector of colliders with their distance.
	vector<NearbyCollider> CollidersInRange(float range, bool myLayerOnly) const;
	vector<ColliderComponent*> CollidersInArea(BoundingBox area, bool myLayerOnly) const;

	inline float GetRadius() const { return max(worldBox.GetSize().x, worldBox.GetSize().y) * 0.5f; }

	inline CollisionLayer GetLayer() const { return layer; }

private:
	void UpdateWorldBox();

	// Bounding box relative to the owners's position.
	BoundingBox relativeBox{};
	BoundingBox worldBox{};
	const CollisionLayer layer{};
};

