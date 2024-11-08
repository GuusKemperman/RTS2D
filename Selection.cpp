#include "precomp.h"
#include "Selection.h"
#include "Entity.h"
#include "EntityManager.h"
#include "SelectingComponent.h"
#include "Scene.h"

Selection::Selection()
{
}

Selection::~Selection()
{
}

void Selection::Select(Scene& scene, const vector<EntityId>& ids)
{
	SetSelectedOfAllTo(scene, false);
	selectedIds = ids;
	SetSelectedOfAllTo(scene, true);
}

void Selection::SetSelectedOfAllTo(Scene& scene, bool value)
{
	for (EntityId ownerId : selectedIds)
	{
		optional<Entity*> entity = scene.entityManager->TryGetEntity(ownerId);

		if (entity)
		{
			SelectingComponent* comp = entity.value()->GetComponent<SelectingComponent>();
			comp->SetIsSelected(value);
		}
	}

	if (!value)
	{
		selectedIds.clear();
	}
}

void Selection::RemoveInvalidIds(Scene& scene)
{
	scene.entityManager->RemoveInvalidIds(selectedIds);
}
