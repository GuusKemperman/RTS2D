#pragma once
#include "Entity.h"
#include "Drawable.h"
#include "BoundingBox.h"
#include "Selection.h"
#include "Group.h"

class Army;
class DynamicCursor;
class Command;
class Action;

class Player :
	public Entity,
	public Drawable
{
public:
	Player(Scene& scene, Army& army);
	~Player();

	void Tick() override;

	// Needed to draw the selection circles.
	void Draw(Surface* target, const Camera* camera) const override;

	void ActionClicked(const string& buttonName);

private:
	void UpdateSelection();
	void UpdateCameraMovement();
	
	void UpdateActionsMenu() const;
	unique_ptr<Command> UpdateGivenCommandIfRelease() const;
	void UpdatePreview();

	void GiveCommand(const unique_ptr<Command>& command);

	void UpdateCursorState(const unique_ptr<Command>& command) const;

	void ResetPreview();

	const Action* ComputeActionAt(uchar menuIndex) const;

	vector<Military*> CheckForMilitary(float2 atPosition) const;
	vector<Military*> CheckForMilitary(BoundingBox inArea) const;

	optional<BoundingBox> ComputeSelectingArea();


	void Highlight(vector<Military*>& toBeHighlighted);
	vector<EntityId> FilterUnselectables(vector<Military*>& toBeSelected);

	void ResetHighlighted();
	vector<Military*> highlightedMilitary{};

	Army& myArmy;

	optional<float2> selectingAreaStart{};
	optional<BoundingBox> selectingArea{};

	//optional<float2> commandMouseStart{};
	//optional<Group::Formation> formationPreview{};

	//static constexpr float adjustRotationTreshold = 100.0f;
	//static constexpr float refreshPreviewCooldown = .1f;
	//float timePreviewCreated = -refreshPreviewCooldown;

	// The first group represents the currentState selection
	array<Selection, 10> storedSelections{};
};

