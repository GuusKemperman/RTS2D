#pragma once
#include "Drawable.h"

class DebugDrawable : 
	public Drawable
{
public:
	DebugDrawable(Scene& scene, bool canZoomOn = true);
	virtual ~DebugDrawable();

	inline void Draw(Surface* target, const Camera* camera) const override { DebugDraw(target, camera); }
	virtual void DebugDraw(Surface* target, const Camera* camera) const = 0;

};