#include "precomp.h"
#include "UIElement.h"
#include "Location.h"
#include "Variable.h"

UIElement::UIElement(Data::Location& location, UIElementType a_type) :
	type(a_type)
{
	const Data::Variable* varX = location.FindVariable("posx");
	localScreenPos.x = varX == nullptr ? 0 : stoi(varX->value);

	Data::Variable* varY = location.FindVariable("posy");
	localScreenPos.y = varY == nullptr ? 0 : stoi(varY->value);

	Data::Variable* varActive = location.FindVariable("isactive");
	isActive = varActive == nullptr ? false : static_cast<bool>(stoi(varActive->value));
}

UIElement::UIElement(UIElementType a_type) :
	type(a_type)
{
}

UIElement::~UIElement()
{
	SetParent(nullptr);

	for (UIElement* child : children)
	{
		SetParent(nullptr);
	}
}

void UIElement::SetParent(UIElement* element)
{
	if (parent != nullptr)
	{
		vector<UIElement*>& siblings = parent->children;
		siblings.erase(remove(siblings.begin(), siblings.end(), this), siblings.end());
	}

	parent = element;
}

bool UIElement::GetIsActive() const
{
	if (parent == nullptr || !isActive)
	{
		return isActive;
	}
	else
	{
		return parent->GetIsActive();
	}
}

int2 UIElement::GetScreenPos() const
{
	if (parent == nullptr)
	{
		return localScreenPos;
	}
	else
	{
		return localScreenPos + parent->GetScreenPos();
	}
}
