#pragma once
#include "OneOff.h"
#include "Drawable.h"

class Smoke :
    public OneOff,
    public Drawable
{
public:
    Smoke(Scene& scene, float2 position, float lifeSpan, float maxRadius);

    void Tick() override;

    void Draw(Surface* target, const Camera* camera) const override;
private:

    static constexpr uint smokeCol = 0x00000000;
    static constexpr float maxAlpha = 64.0f;

    const float2 wind{ -2.0f, -2.0f };
    float2 acceleration{};
    static constexpr float accelerationFactor = 25.0f;
    static constexpr float friction = 0.9f;


    float2 velocity{};
    float maxRadius{};
};

