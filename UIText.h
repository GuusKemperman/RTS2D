#pragma once
#include "UIElement.h"
#include "Drawable.h"

namespace Data
{
    class Location;
}

class UIText :
    public UIElement,
    public Drawable
{
public:
    UIText(Scene& scene, Data::Location& location);
    ~UIText();

    void Draw(Surface* target, const Camera* camera) const override;
private:
    string text{};
    int scale{};
};

