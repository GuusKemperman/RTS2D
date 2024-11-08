#include "precomp.h"
#include "Player.h"

#include "Commands.h"
#include "Group.h"
#include "Military.h"
#include "Camera.h"
#include "Quadtree.h"
#include "TileMap.h"
#include "InputManager.h"
#include "Binding.h"
#include "EntityManager.h"
#include "SelectingComponent.h"
#include "ColliderComponent.h"
#include "TimeManager.h"
#include "Army.h"
#include "Input.h"
#include "Selection.h"
#include "DrawFuncs.h"
#include "DynamicCursor.h"
#include "Scene.h"
#include "UICanvas.h"
#include "UIButton.h"
#include "Action.h"
#include "MySprite.h"


Player::Player(Scene& scene, Army& army) :
	Entity(scene),
	Drawable(scene),
	myArmy(army)
{
	UIElement& actionsNode = scene.canvas->GetElement<UIElement>(string{ "actionsnode" });
	const vector<UIElement*>& children = actionsNode.GetChildren();

	std::function<void(const string&)> func = bind(&Player::ActionClicked, this, placeholders::_1);

	for (UIElement* child : children)
	{
		UIButton* button = dynamic_cast<UIButton*>(child->GetChildren().front());
		
		assert(button);

		button->AddCallback(func);
	}
}

Player::~Player()
{
}

void Player::Tick()
{
	UpdateCameraMovement();

	UpdateSelection();
	storedSelections[0].RemoveInvalidIds(scene);

	UpdateActionsMenu();

	unique_ptr<Command> command = UpdateGivenCommandIfRelease();

	//UpdatePreview();
	UpdateCursorState(command);

	if (command != nullptr
		&& InputManager::GetBinding("command").IsReleased())
	{
		GiveCommand(command);
	}

	ResetHighlighted();
}

void Player::UpdateSelection()
{
	InputManager& inp = InputManager::Inst();
	Selection& activeSelection = storedSelections[0];

	// Storing and retrieving selections
	for (uint keyCode = GLFW_KEY_1; keyCode <= GLFW_KEY_9; keyCode++)
	{
		InputID id = InputManager::ToInputID(keyCode, InputManager::glfwType::key);
		if (!inp.GetInput(id).down)
		{
			continue;
		}

		Selection& storedSelection = storedSelections[static_cast<size_t>(keyCode) - GLFW_KEY_1 + 1];

		static InputID shiftId = InputManager::ToInputID(GLFW_KEY_LEFT_SHIFT, InputManager::glfwType::key);
		if (inp.GetInput(shiftId).held)
		{
			// Store the active selection
			storedSelection.SetSelectedIds(activeSelection.GetSelectedIds());
			return;
		}

		// Retrieve a selection
		activeSelection.SetSelectedOfAllTo(scene, false);
		activeSelection.SetSelectedIds(storedSelection.GetSelectedIds());
		activeSelection.SetSelectedOfAllTo(scene, true);
		return;
	}

	UIElementType mouseHoveringOver = scene.canvas->GetMouseHoveringOverType();

	Binding selectInput = inp.GetBinding("select");

	bool pressed = selectInput.IsPressed(),
		held = selectInput.IsHeld(),
		released = selectInput.IsReleased();
	int2 mousePos = inp.GetMousePos();
	float2 mouseWorld = scene.camera->RealScreenToWorld(mousePos);

	const BoundingBox& worldBox = scene.quadTree->GetMaxBounds();

	if (!worldBox.Contains(mouseWorld))
	{
		return;
	}

	if (pressed)
	{
		if (mouseHoveringOver == UIElementType::null)
		{
			// Start new selecting area
			selectingAreaStart = mouseWorld;
		}
		else
		{
			selectingAreaStart.reset();
		}
	}

	selectingArea = ComputeSelectingArea();

	if (!selectingArea.has_value())
	{
		if (mouseHoveringOver == UIElementType::null)
		{
			// Mouse could be hovering over entities, highlight them.
			vector<Military*> military = CheckForMilitary(mouseWorld);

			if (pressed)
			{
				activeSelection.Select(scene, FilterUnselectables(military));
				selectingArea.reset();
				selectingAreaStart.reset();
			}
			else
			{
				Highlight(military);
			}
		}

		// No selecting area
		return;
	}

	if (held)
	{
		// Highlight all in selecting area
		vector<Military*> military = CheckForMilitary(selectingArea.value());
		Highlight(military);
	}
	else if (released)
	{
		// Select
		vector<Military*> military = CheckForMilitary(selectingArea.value());
		activeSelection.Select(scene, FilterUnselectables(military));
		selectingArea.reset();
		selectingAreaStart.reset();
	}
}

void Player::UpdateCameraMovement()
{
	InputManager& inp = InputManager::Inst();
	float2 axis{};

	// Camera movement
	if (inp.GetBinding("movecamleft").IsHeld())
	{
		axis.x -= 1.0f;
	}
	if (inp.GetBinding("movecamright").IsHeld())
	{
		axis.x += 1.0f;
	}
	if (inp.GetBinding("movecamup").IsHeld())
	{
		axis.y -= 1.0f;
	}
	if (inp.GetBinding("movecamdown").IsHeld())
	{
		axis.y += 1.0f;
	}


	Camera* cam = scene.camera.get();
	float deltaTime = TimeManager::GetDeltaTime();

	static constexpr float scrollSensitivity = .05f;
	static constexpr float moveSensitivity = 250.0f;

	cam->AdjustDesiredZoom(inp.GetMouseWheel() * scrollSensitivity * cam->GetZoom());
	cam->AdjustDesiredPosition(axis * moveSensitivity * deltaTime);
}

void Player::UpdateActionsMenu() const
{
	const Selection& activeSelection = storedSelections[0];

	for (uchar index = 0; index < Military::numOfActions; index++)
	{
		const Action* toDisplay = ComputeActionAt(index);

		UIElement* element = scene.canvas->GetElement<UIButton>("action" + to_string(static_cast<int>(index))).GetParent();
		UISprite& sprite = dynamic_cast<UISprite&>(*element->GetChildren().back());

		element->SetActive(toDisplay != nullptr);

		if (toDisplay == nullptr)
		{
			continue;
		}

		sprite.sprite->SetFrame(toDisplay->frame);
	}
}

unique_ptr<Command> Player::UpdateGivenCommandIfRelease() const
{
	const Selection& activeSelection = storedSelections[0];
	const vector<EntityId>& selectedIds = activeSelection.GetSelectedIds();

	if (selectedIds.empty())
	{
		return nullptr;
	}

	UIElementType hoveringOver = scene.canvas->GetMouseHoveringOverType();

	if (hoveringOver != UIElementType::null)
	{
		return nullptr;
	}

	Military* target{};
	const EntityManager& entityManager = *scene.entityManager;

	for (Military* highlighted : highlightedMilitary)
	{
		if (highlighted->GetArmyId() != ArmyId::player)
		{
			return move(make_unique<Attack>(highlighted->GetId()));
		}
	}

	InputManager& inp = InputManager::Inst();
	float2 mouseWorld = scene.camera->RealScreenToWorld(inp.GetMousePos());
	//optional<float> formationRotation{};

	//if (commandMouseStart.has_value())
	//{
	//	float2 deltaMouse = mouseWorld - commandMouseStart.value();

	//	if (sqrLength(deltaMouse) >= adjustRotationTreshold * adjustRotationTreshold)
	//	{
	//		formationRotation = Float2ToAngle(deltaMouse);
	//	}
	//}

	return move(make_unique<MoveTo>(mouseWorld, AggroLevel::aggressive, optional<float>()));
}

void Player::UpdatePreview()
{
	//if (giveOnInput == nullptr || giveOnInput->type != CommandType::moveTo)
	//{
	//	ResetPreview();
	//	return;
	//}

	//Binding b = InputManager::GetBinding("command");
	//if (!b.IsHeld() && !b.IsPressed())
	//{
	//	return;
	//}

	//Selection& activeSelection = storedSelections[0];
	//MoveTo* moveTo = dynamic_cast<MoveTo*>(giveOnInput.get());

	//float currentTime = TimeManager::GetTotalTimePassed();
	//if (!formationPreview.has_value() || timePreviewCreated - currentTime >= refreshPreviewCooldown)
	//{
	//	formationPreview = Group(scene, activeSelection.selectedIds, false).CalculateFormation(*moveTo);
	//	timePreviewCreated = currentTime;
	//}
	//else if (moveTo->rotationAtEnd.has_value())
	//{
	//	formationPreview.value().rotation = moveTo->rotationAtEnd.value();
	//}
}

void Player::GiveCommand(const unique_ptr<Command>& command)
{
	Selection& activeSelection = storedSelections[0];
	
	const vector<EntityId>& selectedIds = activeSelection.GetSelectedIds();

	if (selectedIds.empty())
	{
		return;
	}

	assert(command != nullptr);

	switch (command->type)
	{
	case CommandType::null:
	{
		break;
	}
	case CommandType::moveTo:
	{
		MoveTo* moveTo = dynamic_cast<MoveTo*>(command.get());

		Group& group = scene.entityManager->AddEntity<Group>(selectedIds, true);

		const Group::Formation& formation = group.CalculateFormation(*moveTo);
		group.FormFormation(formation);
		break;
	}
	case CommandType::attack:
	{
		Attack* attackCommand = dynamic_cast<Attack*>(command.get());

		for (EntityId id : selectedIds)
		{
			Military* military = dynamic_cast<Military*>(scene.entityManager->TryGetEntity(id).value_or(nullptr));

			if (military == nullptr)
			{
				continue;
			}

			military->CommandAttack(*attackCommand);
		}
		break;
	}

	default:
		LOGWARNING("Undefined command type");
		break;
	}
}

void Player::UpdateCursorState(const unique_ptr<Command>& command) const
{
	DynamicCursor& cursor = scene.canvas->GetElement<DynamicCursor>("cursor");

	if (selectingArea.has_value())
	{
		cursor.SetState(DynamicCursor::State::selecting);
		return;
	}
	
	if (command != nullptr)
	{
		if (command->type == CommandType::attack)
		{
			cursor.SetState(DynamicCursor::State::previewAttack);
			return;
		}
		else if (command->type == CommandType::moveTo)
		{
			cursor.SetState(DynamicCursor::State::previewMove);
			return;
		}
	}

	UIElementType hoveringOver = scene.canvas->GetMouseHoveringOverType();

	if (hoveringOver != UIElementType::null)
	{
		if (hoveringOver == UIElementType::button)
		{
			cursor.SetState(DynamicCursor::State::canClickButton);
		}
		else
		{
			cursor.SetState(DynamicCursor::State::idleUI);
		}
		return;
	}

	cursor.SetState(DynamicCursor::State::idleNormal);
}

void Player::ResetPreview()
{
	//formationPreview.reset();
	//timePreviewCreated = 0.0f;
}

const Action* Player::ComputeActionAt(uchar menuIndex) const
{
	const Action* action{};
	const vector<EntityId>& selectedIds = storedSelections[0].GetSelectedIds();

	for (EntityId id : selectedIds)
	{
		Military* military = dynamic_cast<Military*>(scene.entityManager->TryGetEntity(id).value_or(nullptr));

		if (military == nullptr)
		{
			continue;
		}

		const Action* toCompare = military->GetAction(menuIndex);
		if (action == nullptr)
		{
			action = toCompare;
			continue;
		}

		// Actions for all the entities should be the same.
		if (toCompare != action)
		{
			return nullptr;
		}
	}

	return action;
}

vector<Military*> Player::CheckForMilitary(float2 atPosition) const
{
	return CheckForMilitary(BoundingBox{ atPosition, float2{1.0f} });
}

vector<Military*> Player::CheckForMilitary(BoundingBox inArea) const
{
	vector<ColliderComponent*> colliders = scene.quadTree->Query(inArea, allLayersMask);
	vector<Military*> found;

	for (const ColliderComponent* collider : colliders)
	{
		Military* owner = dynamic_cast<Military*>(collider->GetOwner());

		if (owner != nullptr)
		{
			found.push_back(owner);
		}
	}

	return found;
}

optional<BoundingBox> Player::ComputeSelectingArea()
{
	if (!selectingAreaStart.has_value())
	{
		selectingArea.reset();
		return optional<BoundingBox>();
	}

	Binding selectInput = InputManager::Inst().GetBinding("select");
	bool pressed = selectInput.IsPressed(),
		held = selectInput.IsHeld(),
		released = selectInput.IsReleased();

	if (!held && !released)
	{
		selectingAreaStart.reset();
		return optional<BoundingBox>();
	}

	int2 mousePos = InputManager::GetMousePos();
	
	if (mousePos.x < 0
		|| mousePos.x >= SCRWIDTH
		|| mousePos.y < 0
		|| mousePos.y >= SCRHEIGHT)
	{
		return optional<BoundingBox>();
	}

	float2 mouseWorld = scene.camera->RealScreenToWorld(mousePos);

	const BoundingBox& worldBox = scene.quadTree->GetMaxBounds();

	float2 topLeft(min(mouseWorld.x, selectingAreaStart.value().x), min(mouseWorld.y, selectingAreaStart.value().y));
	float2 bottomRight(max(mouseWorld.x, selectingAreaStart.value().x), max(mouseWorld.y, selectingAreaStart.value().y));

	float2 size(max(bottomRight.x - topLeft.x, 1.0f), max(bottomRight.y - topLeft.y, 1.0f));
	BoundingBox proposedArea{ topLeft, size };

	if (!worldBox.Contains(proposedArea))
	{
		return optional<BoundingBox>();
	}

	return proposedArea;
}

void Player::Highlight(vector<Military*>& toBeHighlighted)
{
	for (Military* military : toBeHighlighted)
	{
		SelectingComponent* selectingComp = military->GetComponent<SelectingComponent>();

		if (selectingComp == nullptr)
		{
			continue;
		}

		EntityId ownerId = selectingComp->GetOwner()->GetId();
		highlightedMilitary.push_back(military);
		selectingComp->SetIsHighlighted(true);
	}
}

vector<EntityId> Player::FilterUnselectables(vector<Military*>& toBeSelected)
{
	vector<EntityId> selection{};

	for (Military* military : toBeSelected)
	{
		if (military->GetArmyId() != myArmy.armyId)
		{
			continue;
		}
		
		SelectingComponent* selectingComp = military->GetComponent<SelectingComponent>();

		if (selectingComp == nullptr)
		{
			continue;
		}

		EntityId ownerId = selectingComp->GetOwner()->GetId();

		selection.push_back(ownerId);
	}

	return selection;
}

void Player::ResetHighlighted()
{
	// Resets all the highlighted entities to no longer be highlighted.
	for (Military* military : highlightedMilitary)
	{
		military->GetComponent<SelectingComponent>()->SetIsHighlighted(false);
	}
	highlightedMilitary.clear();
}

void Player::Draw(Surface* target, const Camera* camera) const
{
	if (selectingArea.has_value())
	{
		// First make it to screen pos
		int2 start = camera->WorldToVirtualScreen(selectingArea.value().GetTopLeft());
		int2 end = start + make_int2(selectingArea.value().GetSize());

		DrawFuncs::DrawRectangle(target, start, end, 0x40404080);
	}

	//if (formationPreview)
	//{
	//	float2 vec = AngleToFloat2(formationPreview.value().rotation);

	//	for (const Group::FormationPoint& fp : formationPreview.value().points)
	//	{
	//		int2 centre = camera->WorldToVirtualScreen(fp.position);
	//		optional<Entity*> entity = scene.entityManager->TryGetEntity(fp.assignedUnit);

	//		if (!entity.has_value())
	//		{
	//			continue;
	//		}

	//		int radius = static_cast<int>(entity.value()->GetComponent<ColliderComponent>()->GetRadius());

	//		DrawFuncs::DrawCircle(target, centre, radius, 0x400000ff);

	//		int2 start = centre + make_int2(vec * static_cast<float>(radius));
	//		int2 end = start + make_int2(vec * static_cast<float>(radius / 2));

	//		target->Line(static_cast<float>(start.x), static_cast<float>(start.y), static_cast<float>(end.x), static_cast<float>(end.y), 0xff0000ff);
	//	}
	//}
}

void Player::ActionClicked(const string& buttonName)
{
	Selection& activeSelection = storedSelections[0];
	const vector<EntityId>& selectedIds = activeSelection.GetSelectedIds();

	if (selectedIds.empty())
	{
		return;
	}

	LOGMESSAGE(buttonName);
	
	uint index = static_cast<uchar>(stoi(buttonName.substr(6, buttonName.size() - 5)));

	const Action* toExecute = ComputeActionAt(index);

	if (toExecute == nullptr)
	{
		return;
	}

	for (EntityId id : selectedIds)
	{
		Military* military = dynamic_cast<Military*>(scene.entityManager->TryGetEntity(id).value_or(nullptr));

		if (military == nullptr)
		{
			continue;
		}

		military->PerformAction(toExecute);
	}
}
