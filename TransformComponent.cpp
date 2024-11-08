#include "precomp.h"
#include "Camera.h"
#include "TransformComponent.h"
#include "Entity.h"
#include "Scene.h"

TransformComponent::TransformComponent(Entity* owner, float2 startPos, float startRotation) : Component(owner, ComponentType::transform)
{
    SetWorldPos(startPos);
    SetRotation(startRotation);
}

void TransformComponent::SetWorldPos(float2 newWorldPos)
{
    if (worldPos == newWorldPos)
    {
        return;
    }

    worldPos = newWorldPos;
    Notify();
}

void TransformComponent::SetRotation(float newRotation)
{
    rotation = NormalizeRotation(newRotation);
}

void TransformComponent::RotateTowards(float desiredRotation, float turnSpeed)
{
    desiredRotation = NormalizeRotation(desiredRotation);

    float distanceToAngle = DistanceBetweenAngles(desiredRotation, rotation);

    if (distanceToAngle == 0.0f)
    {
        return;
    }

    float rotateBy = distanceToAngle > 0.0f ? turnSpeed : -turnSpeed;

    if (rotateBy > 0.0f)
    {
        rotateBy = min(distanceToAngle, rotateBy);
    }
    else
    {
        rotateBy = max(distanceToAngle, rotateBy);
    }

    SetRotation(rotation + rotateBy);
}

void TransformComponent::MoveForward(float distanceToMove)
{
    float2 deltaPos = AngleToFloat2(rotation) * distanceToMove;
    TranslateWorldPos(deltaPos);
}

int2 TransformComponent::GetVirtualScreenPos() const
{
    return GetOwner()->scene.camera->WorldToVirtualScreen(worldPos);
}