#include "precomp.h"
#include "SelectingComponent.h"
#include "Entity.h"

SelectingComponent::SelectingComponent(Entity* owner) : Component(owner, ComponentType::selecting)
{

}

SelectingComponent::~SelectingComponent()
{
    if (isSelected)
    {
        LOGWARNING("Selected entity got destroyed");
    }
}