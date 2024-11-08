#pragma once
#include "UIButton.h"
#include "Drawable.h"

class MySprite;

class UIMiniMap :
    public UIButton
{
public:
    UIMiniMap(Scene& scene, Data::Location& location);
    ~UIMiniMap();

    void Draw(Surface* target, const Camera* camera) const override;

    void Click() const override;

    void AddPoint(float2 worldPos, uint col);

private:
    static constexpr uint backgroundCol = 0xEA9E4D;
    static constexpr int plotSize = 2;

    unique_ptr<MySprite> sprite;
    Scene& scene;
};

