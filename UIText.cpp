#include "precomp.h"
#include "UIText.h"
#include "MySprite.h"
#include "Location.h"
#include "Variable.h"

UIText::UIText(Scene& scene, Data::Location& location) :
	UIElement(location, UIElementType::text),
	Drawable(scene, UIElement::uiDrawPriority, true, false)
{
	text = location.FindVariable("text")->value;
	
	Data::Variable* varScale = location.FindVariable("scale");

	scale = varScale == nullptr ? 1 : stoi(varScale->value);
}

UIText::~UIText()
{
}

void UIText::Draw(Surface* target, const Camera* camera) const
{
	if (!GetIsActive())
	{
		return;
	}


	shared_ptr<Surface> printedText = make_shared<Surface>(static_cast<int>(text.size()) * 6, 6);
	printedText->Clear(0x0);
	printedText->Print(text.c_str(), 0, 0, 0xffffff);

	int2 screenPos = GetScreenPos() - int2((printedText->width * scale) / 2, (printedText->height * scale / 2));

	MySprite sprite(move(printedText), 1, 1);

	sprite.DrawScaled(target, screenPos, scale);
}
