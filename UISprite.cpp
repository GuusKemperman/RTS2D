#include "precomp.h"
#include "UISprite.h"
#include "MySprite.h"
#include "BoundingBox.h"
#include "Variable.h"
#include "Location.h"

UISprite::UISprite(Scene& scene, Data::Location& location) :
	Drawable(scene, UIElement::uiDrawPriority, true, false),
	UIElement(location, UIElementType::sprite)
{
	const string& spriteType = location.FindVariable("spritetype")->value;
	sprite = make_unique<MySprite>(spriteType);
}

UISprite::UISprite(Scene& scene, const string& spriteType) :
	Drawable(scene, UIElement::uiDrawPriority, true, false),
	UIElement(UIElementType::sprite)
{
	sprite = make_unique<MySprite>(spriteType);
}

UISprite::~UISprite()
{
}

void UISprite::Draw(Surface* target, const Camera* camera) const
{
	if (GetIsActive())
	{
		sprite->Draw(target, GetScreenPos());
	}
}

bool UISprite::ContainsPoint(int2 point) const
{
	int2 screenPos = GetScreenPos();
	BoundingBox box{ make_float2(screenPos), make_float2(sprite->GetFrameSize()) };

	if (!box.Contains(make_float2(point)))
	{
		return false;
	}

	uint2 relativePosition = make_uint2(point - screenPos);
	return sprite->SamplePixel(relativePosition) & 0xffffff;
}
