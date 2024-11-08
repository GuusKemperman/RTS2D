#include "precomp.h"
#include "BoundingBox.h"

float2 BoundingBox::ComputeRandomPointInsideBox(float minDistFromEdge) const
{
	return float2{ Rand(GetRight() - 2.0f * minDistFromEdge), Rand(GetBottom() - 2.0f * minDistFromEdge) } +
		float2{ minDistFromEdge };
}