#pragma once
#include "DebugDrawable.h"
#include "BoundingBox.h"
#include "enums.h"

// The startingpoint I used: https://github.com/pvigier/Quadtree

class ColliderComponent;

class Quadtree : 
    public DebugDrawable
{
public:
    Quadtree(Scene& scene);
    ~Quadtree();

    void DebugDraw(Surface* target, const Camera* camera) const override;

    inline const BoundingBox& GetMaxBounds() const { return maxBounds; }
    void Resize(BoundingBox newSize);

    void Add(ColliderComponent* collider);
    void Remove(ColliderComponent* collider);

	std::vector<ColliderComponent*> Query(const BoundingBox& box, CollisionLayer layer) const;
private:
	struct Node
	{
		array<unique_ptr<Node>, 4> children;
		vector<ColliderComponent*> values;
	};

    void DrawNode(Node* node, const BoundingBox& nodeBox, Surface* target, const Camera* camera) const;
    void Query(Node* node, const BoundingBox& nodeBox, const BoundingBox& queryBox, CollisionLayer layer, std::vector<ColliderComponent*>& found) const;

    BoundingBox ComputeBox(const BoundingBox& box, int i) const;

    int GetQuadrant(const BoundingBox& nodeBox, const BoundingBox& colliderWorldBox) const;

    void Add(Node* node, std::size_t depth, const BoundingBox& box, ColliderComponent* collider);
    bool Remove(Node* node, const BoundingBox& nodeBox, ColliderComponent* collider);

    void RemoveValue(Node* node, ColliderComponent* collider);

    bool TryMerge(Node* node);

    void Split(Node* node, const BoundingBox& box);

    inline bool IsLeaf(const Node* node) const
    {
        return !static_cast<bool>(node->children[0]);
    }

    static constexpr size_t treshold = 16;
    static constexpr size_t maxDepth = 8;

    BoundingBox maxBounds{};
    std::unique_ptr<Node> startingNode{};
};