#pragma once
#include "Component.h"

// Entities with this component can be selected.
class SelectingComponent : public Component
{
public:
    SelectingComponent(Entity* owner);
    ~SelectingComponent();

    inline bool GetIsSelected() const { return isSelected; }
    inline void SetIsSelected(bool selected) { isSelected = selected; }

    inline bool GetIsHiglighted() const { return isHighlighted; }
    inline void SetIsHighlighted(bool highlighted) { isHighlighted = highlighted; }
private:
    bool isSelected{};

    // Happens just before selection, for example when the cursor is hovering over the entity.
    bool isHighlighted{};
};

