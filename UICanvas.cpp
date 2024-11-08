#include "precomp.h"
#include "UICanvas.h"
#include "PlayerData.h"
#include "Location.h"
#include "Variable.h"
#include "UIElement.h"
#include "UISprite.h"
#include "DynamicCursor.h"
#include "UIButton.h"
#include "InputManager.h"
#include "Input.h"
#include "UIText.h"
#include "UIMiniMap.h"

UICanvas::UICanvas(Scene& a_scene, const string& canvasType) :
	scene(a_scene)
{
	PlayerData data("canvases." + canvasType);
	list<Data::Location*> children = data.GetChildren();

	for (Data::Location* location : children)
	{
		AddElement(location, nullptr);
	}

	unique_ptr<DynamicCursor> cursor = make_unique<DynamicCursor>(scene);
	elementLookUp["cursor"] = cursor.get();
	elements.push_back(move(cursor));
}

UICanvas::~UICanvas()
{
	elements.clear();
	elementLookUp.clear();
}

void UICanvas::Tick()
{
	DynamicCursor& cursor = GetElement<DynamicCursor>("cursor");
	cursor.SetScreenPosition(InputManager::GetMousePos());

	mouseHoveringOver = ComputeMouseHoveringOver();

	UIElementType hoveringOverType = GetMouseHoveringOverType();

	if (hoveringOverType == UIElementType::button)
	{
		cursor.SetState(DynamicCursor::State::canClickButton);

		if (InputManager::GetInput(InputManager::ToInputID(GLFW_MOUSE_BUTTON_1, InputManager::glfwType::mousebutton)).down)
		{
			UIButton* clickedButton = dynamic_cast<UIButton*>(mouseHoveringOver);
			clickedButton->Click();
		}
	}
	else
	{
		cursor.SetState(DynamicCursor::State::idleUI);
	}
}

UIElementType UICanvas::GetMouseHoveringOverType() const
{
	return mouseHoveringOver == nullptr ? UIElementType::null : mouseHoveringOver->type;
}

UIElement* UICanvas::AddElement(Data::Location* location, UIElement* elementParent)
{
	Data::Variable* type = location->FindVariable("type");

	unique_ptr<UIElement> element{};

	if (type == nullptr)
	{
		element = make_unique<UIElement>(*location);
	}
	else if (type->value == "uisprite")
	{
		element = make_unique<UISprite>(scene, *location);
	}
	else if (type->value == "button")
	{
		element = make_unique<UIButton>(scene, *location);
	}
	else if (type->value == "text")
	{
		element = make_unique<UIText>(scene, *location);
	}
	else if (type->value == "minimap")
	{
		element = make_unique<UIMiniMap>(scene, *location);
	}
	else
	{
		assert(true);
	}

	element->SetParent(elementParent);

	list<Data::Location*> children = location->children;

	for (Data::Location* childLocation : children)
	{
		UIElement* child = AddElement(childLocation, element.get());
		element->AddChild(child);
	}

	//assert(elementLookUp.find(location->name) == elementLookUp.end());
	elementLookUp[location->name] = element.get();

	UIElement* toReturn = element.get();
	elements.push_back(move(element));

	return toReturn;
}

UIElement* UICanvas::ComputeMouseHoveringOver() const
{
	int2 mousePos = InputManager::GetMousePos();

	for (const unique_ptr<UIElement>& element : elements)
	{
		if (element->GetIsActive() &&
			element->ContainsPoint(mousePos))
		{
			return element.get();
		}
	}
	return nullptr;
}
