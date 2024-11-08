#include "precomp.h"
#include "ColliderComponent.h"

#include "TileMap.h"
#include "Quadtree.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "Entity.h"
#include "Scene.h"

ColliderComponent::ColliderComponent(Entity* owner, CollisionLayer a_layer) :
	Component(owner, ComponentType::collider),
	layer(a_layer)
{
	TransformComponent* transformComponent = owner->GetComponent<TransformComponent>();
	assert(transformComponent != nullptr);
	transformComponent->AddObserver(this);

	SpriteComponent* sprite = owner->GetComponent<SpriteComponent>();
	assert(sprite != nullptr);
	relativeBox = { float2{0.0f, 0.0f}, make_float2(sprite->GetSize()) };
	worldBox = relativeBox;

	UpdateWorldBox();
	owner->scene.quadTree->Add(this);
}

ColliderComponent::~ColliderComponent()
{
	GetOwner()->scene.quadTree->Remove(this);
}

void ColliderComponent::ObservationMade()
{
	Quadtree* colliders = GetOwner()->scene.quadTree.get();
	colliders->Remove(this);

	UpdateWorldBox();

	colliders->Add(this);
}

vector<ColliderComponent*> ColliderComponent::CollisionsWhenAt(float2 worldPos)
{
	BoundingBox checkingArea = worldBox;
	checkingArea.SetTopLeft(worldPos);
	vector<ColliderComponent*> collisions = GetOwner()->scene.quadTree->Query(checkingArea, layer);
	collisions.erase(remove(collisions.begin(), collisions.end(), this), collisions.end());
	return collisions;
}

vector<ColliderComponent*> ColliderComponent::Collisions()
{
	return CollisionsWhenAt(worldBox.GetTopLeft());
}

vector<NearbyCollider> ColliderComponent::CollidersInRange(float range, bool myLayerOnly) const
{
	float2 myCentre = worldBox.GetCentre();
	float sqrdRange = range * range;
	
	BoundingBox checkingArea(myCentre - float2{ range, range }, float2{ range, range } *2.0f);
	vector<ColliderComponent*> nearbyColliders = CollidersInArea(checkingArea, myLayerOnly);
	vector<NearbyCollider> ret(nearbyColliders.size(), { nullptr, 0.0f });

	size_t retIndex = 0;
	for (size_t i = 0; i < nearbyColliders.size(); ++i)
	{
		ColliderComponent* collider = nearbyColliders[i];
		float sqrdDist = SqrdDistance(myCentre, collider->GetBoundingBox().GetCentre());

		if (sqrdDist <= sqrdRange)
		{
			ret[retIndex++] = NearbyCollider{ collider, sqrtf(sqrdDist) };
		}
	}

	ret.resize(retIndex);
	return ret;
}

vector<ColliderComponent*> ColliderComponent::CollidersInArea(BoundingBox area, bool myLayerOnly) const
{
	CollisionLayer layerToCheck = myLayerOnly ? layer : allLayersMask;
	vector<ColliderComponent*> colliders = GetOwner()->scene.quadTree->Query(area, layerToCheck);
	auto myself = find(colliders.begin(), colliders.end(), this);

	if (myself != colliders.end())
	{
		colliders.erase(myself);
	}
	return colliders;
}

void ColliderComponent::UpdateWorldBox()
{
	worldBox.SetTopLeft(GetOwner()->GetComponent<TransformComponent>()->GetWorldPos());
}

