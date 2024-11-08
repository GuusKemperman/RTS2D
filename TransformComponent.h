#pragma once
#include "Component.h"
#include "Subject.h"

class TransformComponent :
    public Component, 
    public Subject
{
public:
    TransformComponent(Entity* owner, float2 startPos = {0.0f, 0.0f}, float startRotation = 0.0f);

    void SetWorldPos(float2 a_worldPos);
    inline void SetWorldPos(float x, float y) { SetWorldPos(float2{ x, y }); }
    inline void TranslateWorldPos(float2 translation) { SetWorldPos(worldPos + translation); }

    void SetRotation(float a_rotation);

    // Returns rotation after rotating
    void RotateTowards(float rotation, float turnSpeed);
    inline void Rotate(float byAmount) { SetRotation(GetRotation() + byAmount); }

    void MoveForward(float distanceToMove);

    inline const float2& GetWorldPos() const { return worldPos; }
    inline const float& GetRotation() const { return rotation; }

    int2 GetVirtualScreenPos() const; // Has to look up the camera position and zoomfactor first.

private:
    float2 worldPos{};
    float rotation{};
};

