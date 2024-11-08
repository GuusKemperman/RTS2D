#pragma once
#include "enums.h"

namespace Data
{
	class Location;
}

class UIElement
{
public:
	UIElement(Data::Location& location, UIElementType type = UIElementType::null);
	UIElement(UIElementType type = UIElementType::null);
	virtual ~UIElement();

	virtual void Update() {};

	inline UIElement* GetParent() const { return parent; }
	void SetParent(UIElement* element);

	bool GetIsActive() const;
	inline virtual void SetActive(bool active) { isActive = active; }

	inline void SetLocalPosition(int2 pos) { localScreenPos = pos; }
	inline int2 GetLocalScreenPos() const { return localScreenPos; }

	inline void AddChild(UIElement* child) { children.push_back(child); }
	inline const vector<UIElement*>& GetChildren() const { return children; }

	int2 GetScreenPos() const;

	inline virtual bool ContainsPoint(int2 point) const { return false; }

	bool isActive = true;
	bool tickEveryFrame = false;
	const UIElementType type{};

protected:
	static constexpr int uiDrawPriority = 1000;

private:
	UIElement* parent{};
	vector<UIElement*> children{};


	int2 localScreenPos{};
};

