#include "precomp.h"
#include "UIMiniMap.h"
#include "Scene.h"
#include "Quadtree.h"
#include "MySprite.h"
#include "InputManager.h"
#include "Camera.h"

UIMiniMap::UIMiniMap(Scene& a_scene, Data::Location& location) :
	UIButton(a_scene, location),
	scene(a_scene)
{
	sprite = make_unique<MySprite>(move(make_shared<Surface>(size.x, size.y)), 1, 1);
}

UIMiniMap::~UIMiniMap()
{
}



void UIMiniMap::Draw(Surface* target, const Camera* camera) const
{
	if (GetIsActive())
	{
		sprite->Draw(target, GetScreenPos());
		sprite->GetSurface()->Clear(backgroundCol);
	}
}

void UIMiniMap::Click() const
{
	int2 mousePos = InputManager::GetMousePos();
	int2 screenPos = GetScreenPos();

	float2 relativeMouse = make_float2(mousePos - screenPos);

	BoundingBox worldBox = scene.quadTree->GetMaxBounds();

	float worldX = relativeMouse.x / static_cast<float>(size.x) * worldBox.GetSize().x;
	float worldY = relativeMouse.y / static_cast<float>(size.y) * worldBox.GetSize().y;

	scene.camera->SetDesiredCentre({ worldX, worldY });
}

void UIMiniMap::AddPoint(float2 worldPos, uint col)
{
	BoundingBox worldBox = scene.quadTree->GetMaxBounds();

	int valueX = static_cast<int>((worldPos.x / worldBox.GetSize().x) * static_cast<float>(size.x));
	int valueY = static_cast<int>((worldPos.y / worldBox.GetSize().y) * static_cast<float>(size.y));

	for (int x = 0; x < plotSize; x++)
	{
		for (int y = 0; y < plotSize; y++)
		{
			sprite->GetSurface()->Plot(x + valueX, y + valueY, col);
		}
	}
	
}
