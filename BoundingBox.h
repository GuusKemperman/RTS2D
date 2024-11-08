#pragma once

class BoundingBox
{
public:
	BoundingBox() = default;
	BoundingBox(const float2& topLeft, const float2& size)
	{
		left = topLeft.x, top = topLeft.y, width = size.x, height = size.y;
	}
	BoundingBox(const float4& a_values)
	{
		boxValues = a_values;
	}

	inline bool Contains(const float2 point) const
	{
		return point.x >= left
			&& point.x <= GetRight()
			&& point.y >= top
			&& point.y <= GetBottom();
	}

	inline bool Contains(const BoundingBox& box) const
	{
		return left <= box.left 
			&& top <= box.top
			&& box.GetRight() <= GetRight() 
			&& box.GetBottom() <= GetBottom();
	}

	inline bool Intersects(const BoundingBox& box) const
	{
		return left < box.GetRight() 
			&& GetRight() > box.left 
			&& top < box.GetBottom() 
			&& GetBottom() > box.top;
	}

	inline float GetLeft() const { return left;  }
	inline float GetTop() const { return top; }
	inline float GetRight() const { return left + width; }
	inline float GetBottom() const { return top + height; }
	inline float2 GetTopLeft() const { return float2(left, top); }
	inline float2 GetBottomRight() const { return float2(GetRight(), GetBottom()); }

	inline float2 GetSize() const { return float2(width, height); }

	inline float2 GetCentre() const { return float2(left + width / 2.0f, top + height / 2.0f); }
	inline float4& GetBoxValues() { return boxValues; }

	inline void SetTopLeft(const float2& topLeft) { left = topLeft.x, top = topLeft.y; }

	float2 ComputeRandomPointInsideBox(float minDistFromEdge = 0.0f) const;
private:
	union { struct { float left, top, width, height; }; float4 boxValues; };
};

