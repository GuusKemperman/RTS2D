#include "precomp.h"
#include "Camera.h"
#include "Drawable.h"
#include "Scene.h"

Drawable::Drawable(Scene& a_scene, int a_priority, bool shouldRegister, bool a_canZoomOn) : 
	drawToScene(a_scene),
	priority(a_priority),
	canZoomOn(a_canZoomOn)
{
	if (shouldRegister)
	{
		drawToScene.camera->AddDrawable(this);
	}
}

Drawable::~Drawable()
{
	drawToScene.camera->RemoveDrawable(this);
}

void Drawable::SetPriority(int a_priority)
{
	Camera& inst = *drawToScene.camera;
	inst.RemoveDrawable(this);
	priority = a_priority;
	inst.AddDrawable(this);
}