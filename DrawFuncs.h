#pragma once

constexpr uint alphaMask	=	0xff000000;
constexpr uint redMask		=	0x00ff0000;
constexpr uint greenMask	=	0x0000ff00;
constexpr uint blueMask		=	0x000000ff;

using Color = uint;

struct DrawFuncs
{
	inline static uint BlendTransparant(const Color& dst, const Color& src)
	{
		Color srcA = src & alphaMask;

		if (srcA == 0) 
		{ 
			return dst; 
		}

		if (srcA == alphaMask) 
		{ 
			return src; 
		}

		Color srcR = (src & redMask) >> 16;
		Color srcG = (src & greenMask) >> 8;
		Color srcB = src & blueMask;

		Color dstR = (dst & redMask) >> 16;
		Color dstG = (dst & greenMask) >> 8;
		Color dstB = dst & blueMask;

		Color multiplier = 255u - (srcA >> 24);

		Color r = srcR + ((multiplier * (dstR - srcR)) >> 8);
		Color g = srcG + ((multiplier * (dstG - srcG)) >> 8);
		Color b = srcB + ((multiplier * (dstB - srcB)) >> 8);

		return (r << 16) + (g << 8) + b;
	}

	// Draws a filled circle
	static void DrawCircle(Surface* target, int2 pos, int radius, uint col);

	static void DrawCircle(Surface* target, int2 pos, int radius, int lineThickness, uint col);

	static void DrawRectangle(Surface* target, int2 topLeft, int2 bottomRight, uint col);
};
