#pragma once
#include "enums.h"

namespace Data
{
	class Location;
}

class Scene;
class UIElement;

class UICanvas
{
public:
	UICanvas(Scene& scene, const string& canvasType);
	~UICanvas();

	void Tick();

	template<typename T>
	T& GetElement(const string& name);

	template<typename T>
	optional<T*> TryGetElement(const string& name);

	UIElementType GetMouseHoveringOverType() const;

private:
	UIElement* AddElement(Data::Location* location, UIElement* elementParent);

	UIElement* ComputeMouseHoveringOver() const;

	Scene& scene;

	unordered_map<string, UIElement*> elementLookUp;
	vector<unique_ptr<UIElement>> elements;

	UIElement* mouseHoveringOver{};
};

template<typename T>
inline T& UICanvas::GetElement(const string& name)
{
	auto it = elementLookUp.find(name);
	assert(it != elementLookUp.end());

	T* element = dynamic_cast<T*>(it->second);
	assert(element != nullptr);
	return *element;
}

template<typename T>
inline optional<T*> UICanvas::TryGetElement(const string& name)
{
	auto it = elementLookUp.find(name);

	if (it == elementLookUp.end())
	{
		return optional<T*>();
	}

	T* element = dynamic_cast<T*>(it->second);
	assert(element != nullptr);

	return element;
}
