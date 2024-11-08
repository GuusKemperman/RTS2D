#pragma once
#include "UISprite.h"

// Needs to be made part of UI later on.
class DynamicCursor :
    public UISprite
{
public:
    DynamicCursor(Scene& scene);
    ~DynamicCursor();

    void Draw(Surface* target, const Camera* camera) const override;

    enum class State { invisible = -1, previewMove, previewAttack, idleNormal, idleUI, canClickButton, selecting };
    void SetState(State to);

    void SetScreenPosition(int2 screenPos);

    inline bool ContainsPoint(int2 point) const override { return false; }

private:
    static constexpr int scale = 2;
    static constexpr int animFps = 6;
   
    State currentState = State::invisible;
    int2 screenPos{};
};

