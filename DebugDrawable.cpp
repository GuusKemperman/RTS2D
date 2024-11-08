#include "precomp.h"
#include "DebugDrawable.h"

DebugDrawable::DebugDrawable(Scene& scene, bool canZoomOn) :
#ifdef _DEBUG
	Drawable(scene, 10000, true, canZoomOn)
#else
	Drawable(scene, 0, false, canZoomOn)
#endif // _DEBUG

{

}

DebugDrawable::~DebugDrawable()
{
;
}
