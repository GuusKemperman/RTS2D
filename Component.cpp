#include "precomp.h"
#include "Component.h"

Component::Component(Entity* a_owner, ComponentType arg_type) : type{ arg_type }, owner { a_owner }
{

}