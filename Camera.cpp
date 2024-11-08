#include "precomp.h"
#include "Camera.h"
#include "Drawable.h"
#include "TimeManager.h"


Camera::Camera(Surface* a_target) :
	target(a_target)
{
	virtualScreen = new Surface(SCRWIDTH, SCRHEIGHT);
}

Camera::~Camera()
{
}

void Camera::DrawFrame()
{
	// Update the position
	actualZoom = desiredZoom;

	if (lockedInside.has_value())
	{
		ComputeViewport();

		desiredPos.x = clamp(desiredPos.x, lockedInside.value().GetLeft(), lockedInside.value().GetRight() - viewport.GetSize().x);
		desiredPos.y = clamp(desiredPos.y, lockedInside.value().GetTop(), lockedInside.value().GetBottom() - viewport.GetSize().y);
	}

	actualPos = make_int2(desiredPos);
	ComputeViewport();

	float2 topLeft = RealScreenToWorld(int2(0, 0));
	float2 bottomRight = RealScreenToWorld(int2(SCRWIDTH - 1, SCRHEIGHT - 1));
	BoundingBox viewport{ topLeft, bottomRight - topLeft };
	// virtualScreen->Clear(0);

	auto it = drawables.begin();
	for (; it != drawables.end(); it++)
	{
		const Drawable* drawable = *it;
		if (!drawable->canZoomOn)
		{
			break;
		}
		drawable->Draw(virtualScreen, this);
	}

	// Zooming
	if (actualZoom == 1.0f)
	{
		size_t size = target->width * target->height * sizeof(uint);
		memcpy_s(target->pixels, size, virtualScreen->pixels, size);
	}
	else
	{
		ApplyZooming();
	}

	for (; it != drawables.end(); it++)
	{
		const Drawable* drawable = *it;
		drawable->Draw(target, this);
	}
}

void Camera::SetDesiredCentre(float2 centre)
{
	SetDesiredPosition(centre - viewport.GetSize() / 2.0f);
}

void Camera::SetDesiredPosition(float2 a_desiredPos)
{
	if (lockedInside.has_value())
	{
		BoundingBox lockedInsideBox = lockedInside.value();
		BoundingBox proposedViewport = viewport;
		proposedViewport.SetTopLeft(a_desiredPos);

		float4& values = proposedViewport.GetBoxValues();

		values[0] = clamp(values[0], 0.0f, lockedInsideBox.GetRight() - proposedViewport.GetSize().x);
		values[1] = clamp(values[1], 0.0f, lockedInsideBox.GetBottom() - proposedViewport.GetSize().y);

		a_desiredPos = proposedViewport.GetTopLeft();
	}

	desiredPos = a_desiredPos;
}

float2 Camera::RealScreenToWorld(const int2& screenPos) const
{
	float dx = 1.0f / actualZoom;
	int2 virtualScreenPos(static_cast<int>(dx * screenPos.x) % virtualScreen->width, static_cast<int>(dx * screenPos.y) % virtualScreen->height);
	return VirtualScreenToWorld(virtualScreenPos);
}

const BoundingBox& Camera::GetViewport() const
{
	return viewport;
}

void Camera::SetLockInsideBounds(const optional<BoundingBox>& box)
{
	lockedInside = box;
}

void Camera::AdjustDesiredPosition(float2 axis)
{
	SetDesiredPosition(desiredPos + axis);
}

void Camera::AdjustDesiredZoom(float zoom)
{
	desiredZoom = clamp(desiredZoom + zoom, 1.0f, maxZoom);
}

void Camera::SetActualPosition(int2 pos)
{
	if (lockedInside)
	{
		float2 floatPos = make_float2(pos);
		pos = make_int2(static_cast<int>(clamp(floatPos.x, lockedInside.value().GetLeft(), lockedInside.value().GetRight() - SCRWIDTH)),
			static_cast<int>(clamp(floatPos.y, lockedInside.value().GetTop(), lockedInside.value().GetBottom() - SCRHEIGHT)));
	}

	actualPos = pos;
	desiredPos = make_float2(pos);
}

void Camera::AddDrawable(Drawable* drawable)
{
#ifdef _DEBUG
	auto it = find(drawables.begin(), drawables.end(), drawable);
	assert(it == drawables.end());
#endif // _DEBUG

	drawables.insert(drawable);
}

void Camera::RemoveDrawable(Drawable* drawable)
{
	auto it = find(drawables.begin(), drawables.end(), drawable);

	if (it == drawables.end())
	{
		LOGWARNING("Erasing drawable that was not in set.");
		return;
	}
	drawables.erase(it);
}

void Camera::ApplyZooming()
{
	static constexpr uint shift = 20;
	int dx = static_cast<int>((1.0f / actualZoom) * powf(2, shift));
	uint* dst = target->pixels;
	uint* src = virtualScreen->pixels;
	int srcWidth = virtualScreen->width;
	int srcHeight = virtualScreen->height;

	for (int y = 0; y < target->height; y++)
	{
		for (int x = 0; x < target->width; x++, dst++)
		{
			int readxpos = ((dx * x) >> shift) % srcWidth;
			int readypos = ((dx * y) >> shift) % srcHeight;
			*dst = src[readxpos + readypos * srcWidth];
		}
	}
}

void Camera::ComputeViewport()
{
	float2 topLeft = RealScreenToWorld(int2(0, 0));
	float2 bottomRight = RealScreenToWorld(int2(SCRWIDTH - 1, SCRHEIGHT - 1));
	viewport = BoundingBox{ topLeft, bottomRight - topLeft };
}

bool Camera::Comp::operator()(const Drawable* one, const Drawable* two) const
{
	if (one->canZoomOn == two->canZoomOn)
	{
		return one->GetPriority() < two->GetPriority();
	}
	return one->canZoomOn;
}