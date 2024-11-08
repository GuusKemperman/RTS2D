#pragma once
#include "enums.h"

class Scene;

class Selection
{
public:
	Selection();
	~Selection();

	Selection operator=(const Selection& other) = delete;

	inline const vector<EntityId>& GetSelectedIds() const { return selectedIds; }
	inline void SetSelectedIds(const vector<EntityId>& ids) { selectedIds = ids; }

	void Select(Scene& scene, const vector<EntityId>& ids);

	void SetSelectedOfAllTo(Scene& scene, bool value);

	void RemoveInvalidIds(Scene& scene);
	
private:
	vector<EntityId> selectedIds;
};

