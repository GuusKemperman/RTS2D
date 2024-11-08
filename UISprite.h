#pragma once
#include "Drawable.h"
#include "UIElement.h"

class MySprite;

class UISprite :
    public UIElement,
    public Drawable
{
public:
    UISprite(Scene& scene, Data::Location& location);
    UISprite(Scene& scene, const string& spriteType);
    ~UISprite();

    void Draw(Surface* target, const Camera* camera) const override;

    bool ContainsPoint(int2 point) const override;

    unique_ptr<MySprite> sprite;
};

