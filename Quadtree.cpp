#include "precomp.h"
#include "Camera.h"
#include "Quadtree.h"
#include "ColliderComponent.h"
#include "Scene.h"

Quadtree::Quadtree(Scene& a_scene) :
    DebugDrawable(a_scene)
{
}

Quadtree::~Quadtree()
{
    
}

void Quadtree::Resize(BoundingBox newSize)
{
    vector<ColliderComponent*> allColliders{};
    if (maxBounds.GetSize() != float2{ 0.0f, 0.0f })
    {
        vector<ColliderComponent*> allColliders = Query(maxBounds, allLayersMask);
        if (!allColliders.empty())
        {
            LOGWARNING("Quadtree size changing, but there were still " << allColliders.size() << " existing colliders.");
        }
    }

    maxBounds = newSize;
    startingNode = make_unique<Node>();

    for (ColliderComponent* collider : allColliders)
    {
        Add(collider);
    }
}

void Quadtree::DebugDraw(Surface* target, const Camera* camera) const
{
    if (startingNode.get() == nullptr)
    {
        return;
    }
    DrawNode(startingNode.get(), maxBounds, target, camera);
}

void Quadtree::Add(ColliderComponent* collider)
{
    Add(startingNode.get(), 0, maxBounds, collider);
}

void Quadtree::Remove(ColliderComponent* collider)
{
    Remove(startingNode.get(), maxBounds, collider);
}

std::vector<ColliderComponent*> Quadtree::Query(const BoundingBox& box, CollisionLayer layer) const
{
    std::vector values = std::vector<ColliderComponent*>();
    Query(startingNode.get(), maxBounds, box, layer, values);
    return values;
}

void Quadtree::DrawNode(Node* node, const BoundingBox& nodeBox, Surface* target, const Camera* camera) const
{
    int2 nodetopLeft = camera->WorldToVirtualScreen(nodeBox.GetTopLeft());
    int2 nodeSize = make_int2(nodeBox.GetSize());
    target->Box(nodetopLeft.x, nodetopLeft.y, nodetopLeft.x + nodeSize.x, nodetopLeft.y + nodeSize.y, 0x808080);

    for (ColliderComponent* collider : node->values)
    {
        BoundingBox box = collider->GetBoundingBox();
        int2 topLeft = camera->WorldToVirtualScreen(box.GetTopLeft());
        int2 size = make_int2(box.GetSize());
        target->Box(topLeft.x, topLeft.y, topLeft.x + size.x, topLeft.y + size.y, 0x404040);
    }
    if (!IsLeaf(node))
    {
        for (size_t i = 0; i < node->children.size(); i++)
        {
            BoundingBox childBox = ComputeBox(nodeBox, static_cast<int>(i));
            DrawNode(node->children[i].get(), childBox, target, camera);
        }
    }
}

void Quadtree::Query(Node* node, const BoundingBox& nodeBox, const BoundingBox& queryBox, CollisionLayer layer, std::vector<ColliderComponent*>& found) const
{
    assert(node != nullptr);

    if (!queryBox.Intersects(nodeBox))
    {
        assert(queryBox.Intersects(nodeBox));
    }

    for (ColliderComponent* collider : node->values)
    {
        if ((collider->GetLayer() & layer) != 0
            && queryBox.Intersects(collider->GetBoundingBox()))
        {
            found.push_back(collider);
        }
    }
    if (!IsLeaf(node))
    {
        for (size_t i = 0; i < node->children.size(); i++)
        {
            BoundingBox childBox = ComputeBox(nodeBox, static_cast<int>(i));
            if (queryBox.Intersects(childBox))
            {
                Query(node->children[i].get(), childBox, queryBox, layer, found);
            }
        }
    }
}

BoundingBox Quadtree::ComputeBox(const BoundingBox& box, int i) const
{
    float2 origin = box.GetTopLeft();
    float2 childSize = box.GetSize() / 2.0f;
    switch (i)
    {
        // North West
    case 0:
        return BoundingBox(origin, childSize);
        // Norst East
    case 1:
        return BoundingBox(float2(origin.x + childSize.x, origin.y), childSize);
        // South West
    case 2:
        return BoundingBox(float2(origin.x, origin.y + childSize.y), childSize);
        // South East
    case 3:
        return BoundingBox(origin + childSize, childSize);
    default:
        assert(false && "Invalid child index");
        return BoundingBox();
    }
}

int Quadtree::GetQuadrant(const BoundingBox& nodeBox, const BoundingBox& colliderWorldBox) const
{
    const float2& centre = nodeBox.GetCentre();
    

    // West
    if (colliderWorldBox.GetRight() < centre.x)
    {
        // North West
        if (colliderWorldBox.GetBottom() < centre.y)
            return 0;
        // South West
        else if (colliderWorldBox.GetTop() >= centre.y)
            return 2;
        // Not contained in any quadrant
        else
            return -1;
    }
    // East
    else if (colliderWorldBox.GetLeft() >= centre.x)
    {
        // North East
        if (colliderWorldBox.GetBottom() < centre.y)
            return 1;
        // South East
        else if (colliderWorldBox.GetTop() >= centre.y)
            return 3;
        // Not contained in any quadrant
        else
            return -1;
    }
    // Not contained in any quadrant
    else
        return -1;
}

void Quadtree::Add(Node* node, std::size_t depth, const BoundingBox& box, ColliderComponent* collider)
{
    assert(node != nullptr);

    if (!box.Contains(collider->GetBoundingBox()))
    {
        assert(box.Contains(collider->GetBoundingBox()));
    }

    if (IsLeaf(node))
    {
        // Insert the value in this node if possible
        if (depth >= maxDepth || node->values.size() < treshold)
        {
            node->values.push_back(collider);
        }
        // Otherwise, we split and we try again
        else
        {
            Split(node, box);
            Add(node, depth, box, collider);
        }
    }
    else
    {
        int i = GetQuadrant(box, collider->GetBoundingBox());
        // Add the value in a child if the value is entirely contained in it
        if (i != -1)
        {
            Add(node->children[static_cast<std::size_t>(i)].get(), depth + 1, ComputeBox(box, i), collider);
        }
        // Otherwise, we add the value in the currentState node
        else
        {
            node->values.push_back(collider);
        }
    }
}

bool Quadtree::Remove(Node* node, const BoundingBox& nodeBox, ColliderComponent* collider)
{
    assert(node != nullptr);
    if (!nodeBox.Contains(collider->GetBoundingBox()))
    {
        assert(nodeBox.Contains(collider->GetBoundingBox()));
    }

    if (IsLeaf(node))
    {
        // Remove the colliderWorldBox from node
        RemoveValue(node, collider);
        return true;
    }
    else
    {
        // Remove the colliderWorldBox in a child if the colliderWorldBox is entirely contained in it
        auto i = GetQuadrant(nodeBox, collider->GetBoundingBox());
        if (i != -1)
        {
            if (Remove(node->children[static_cast<std::size_t>(i)].get(), ComputeBox(nodeBox, i), collider))
            {
                return TryMerge(node);
            }
        }
        // Otherwise, we remove the colliderWorldBox from the currentState node
        else
        {
            RemoveValue(node, collider);
        }
        return false;
    }
}

void Quadtree::RemoveValue(Node* node, ColliderComponent* collider)
{
    // Find the colliderWorldBox in node->values
    auto it = std::find(std::begin(node->values), std::end(node->values), collider);
    assert(it != std::end(node->values) && "Trying to remove a colliderWorldBox that is not present in the node");
    // Swap with the last element and pop back
    *it = std::move(node->values.back());
    node->values.pop_back();
}

bool Quadtree::TryMerge(Node* node)
{
    assert(node != nullptr);
    assert(!IsLeaf(node) && "Only interior nodes can be merged");
    auto nbValues = node->values.size();
    for (const auto& child : node->children)
    {
        if (!IsLeaf(child.get()))
        {
            return false;
        }
        nbValues += child->values.size();
    }
    if (nbValues <= treshold)
    {
        node->values.reserve(nbValues);
        // Merge the values of all the children
        for (const auto& child : node->children)
        {
            for (const auto& value : child->values)
                node->values.push_back(value);
        }
        // Remove the children
        for (auto& child : node->children)
            child.reset();
        return true;
    }
    else
        return false;
}

void Quadtree::Split(Node* node, const BoundingBox& box)
{
    assert(node != nullptr);
    assert(IsLeaf(node) && "Only leaves can be split");

    // Create children
    for (unique_ptr<Node>& child : node->children)
    {
        child = std::make_unique<Node>();
    }
    // Assign values to children
    vector<ColliderComponent*> newValues = std::vector<ColliderComponent*>(); // New values for this node
    for (ColliderComponent* collider : node->values)
    {
        int i = GetQuadrant(box, collider->GetBoundingBox());
        if (i != -1)
        {
            node->children[i]->values.push_back(collider);
        }
        else
        {
            newValues.push_back(collider);
        }
    }
    node->values = move(newValues);
}
