#pragma once
#include "UIElement.h"
#include "DebugDrawable.h"

#include <functional>
#include "BoundingBox.h"

class UIButton :
    public UIElement,
    public Drawable
{
public:
    UIButton(Scene& scene, Data::Location& location);
    ~UIButton();
    
    virtual void Draw(Surface* target, const Camera* camera) const override;

    BoundingBox GetBoundingBox() const;
    bool ContainsPoint(int2 point) const override;

    virtual void Click() const;
    
    void AddCallback(function<void(const string&)> callback);

protected:
    std::function<void(const string&)> callback{};

    int2 size{};
    string name{};
};

