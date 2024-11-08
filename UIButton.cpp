#include "precomp.h"
#include "UIButton.h"
#include "DrawFuncs.h"
#include "Location.h"
#include "Variable.h"

UIButton::UIButton(Scene& scene, Data::Location& location) :
    Drawable(scene, uiDrawPriority, true, false),
    UIElement(location, UIElementType::button)
{
    name = location.name;
    size.x = stoi(location.FindVariable("sizex")->value);
    size.y = stoi(location.FindVariable("sizey")->value);
}

UIButton::~UIButton()
{
}

void UIButton::Draw(Surface* target, const Camera* camera) const
{
#ifdef _DEBUG
    if (GetIsActive())
    {
        BoundingBox box = GetBoundingBox();
        int2 topLeft = make_int2(box.GetTopLeft()),
            bottomRight = make_int2(static_cast<int>(box.GetRight()), static_cast<int>(box.GetBottom()));
        target->Box(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, 0xffffffff);
    }
#endif // _DEBUG
}

BoundingBox UIButton::GetBoundingBox() const
{
    float2 screenPos = make_float2(GetScreenPos());
    return BoundingBox(float4{ screenPos.x, screenPos.y, static_cast<float>(size.x), static_cast<float>(size.y) });
}

bool UIButton::ContainsPoint(int2 point) const
{
    return GetBoundingBox().Contains(make_float2(point));
}

void UIButton::Click() const
{
    callback(name);
}

void UIButton::AddCallback(function<void(const string&)> a_callback)
{
    callback = a_callback;
}
