#pragma once

class ColliderComponent;

struct NearbyCollider
{
	ColliderComponent* component{};
	float distance{};
};
