#pragma once
#include "BoundingBox.h"

class Drawable;

class Camera
{
public:
	Camera(Surface* target);
	~Camera();

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	void DrawFrame();
	
	const BoundingBox& GetViewport() const;
	void SetLockInsideBounds(const optional<BoundingBox>& box);
	
	void SetDesiredCentre(float2 centre);
	void SetDesiredPosition(float2 pos);
	void SetActualPosition(int2 pos);

	// Moves the desired centre based on the cameras movement speed. A given axis should be between -1.0f and 1.0f;
	void AdjustDesiredPosition(float2 axis);
	void AdjustDesiredZoom(float byAmount);

	inline float2 GetDesiredCentre() const { return desiredPos; }
	inline int2 GetActualPosition() const { return actualPos; }
	inline float GetZoom() const { return actualZoom; }

	float2 RealScreenToWorld(const int2& screenPos) const;

	inline float2 VirtualScreenToWorld(const int2& screenPos) const;
	inline int2 WorldToVirtualScreen(const float2& worldPos) const;

	void AddDrawable(Drawable* drawable);
	void RemoveDrawable(Drawable* drawable);
private:
	void ApplyZooming();
	void ComputeViewport();

	Surface* target{};
	Surface* virtualScreen{};

	struct Comp
	{
		bool operator()(const Drawable* one, const Drawable* two) const;
	};
	multiset<Drawable*, Comp> drawables{};

	float2 desiredPos{}; // The camera will smoothly move to the actual position
	int2 actualPos{}; // 
	
	static constexpr float maxZoom = 2.5f;
	float desiredZoom{1.0f};
	float actualZoom{1.0f};

	BoundingBox viewport{};
	optional<BoundingBox> lockedInside{};
};

float2 Camera::VirtualScreenToWorld(const int2& screenPos) const 
{ 
	return make_float2(actualPos + screenPos); 
}
int2 Camera::WorldToVirtualScreen(const float2& worldPos) const 
{
	return int2(static_cast<int>(worldPos.x), static_cast<int>(worldPos.y)) - actualPos; 
}