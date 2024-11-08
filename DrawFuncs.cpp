#include "precomp.h"
#include "DrawFuncs.h"

    void DrawFuncs::DrawCircle(Surface* target, int2 pos, int radius, uint col)
    {
        int endX = min(pos.x + radius, target->width),
            endY = min(pos.y + radius, target->height);

        int outerRadiusSqrd = radius * radius;

        uint srcAlpha = (col & alphaMask) >> 24;
        uint src = ScaleColor(col, srcAlpha);
        uint dstAlpha = 255u - srcAlpha;
        


        for (int y = max(pos.y - radius, 0); y < endY; y++)
        {
            for (int x = max(pos.x - radius, 0); x < endX; x++)
            {
                int dx = pos.x - x, dy = pos.y - y;
                int distSqrd = dx * dx + dy * dy;
                if (distSqrd <= outerRadiusSqrd)
                {
                    uint& dst = target->pixels[x + y * target->width];
                    dst = ScaleColor(dst, dstAlpha) + src;
                }
            }
        }
    }

    void DrawFuncs::DrawCircle(Surface* target, int2 pos, int radius, int lineThickness, uint col)
    {
        int endX = min(pos.x + radius, target->width),
            endY = min(pos.y + radius, target->height);

        int outerRadiusSqrd = radius * radius,
            innerRadiusSqrd = sqr(radius - lineThickness);

        uint srcAlpha = (col & alphaMask) >> 24;
        uint src = ScaleColor(col, srcAlpha);
        uint dstAlpha = 255u - srcAlpha;

        for (int y = max(pos.y - radius, 0); y < endY; y++)
        {
            for (int x = max(pos.x - radius, 0); x < endX; x++)
            {
                int dx = pos.x - x, dy = pos.y - y;
                int distSqrd = dx * dx + dy * dy;
                if (distSqrd <= outerRadiusSqrd && distSqrd >= innerRadiusSqrd)
                {
                    uint& dst = target->pixels[x + y * target->width];
                    dst = ScaleColor(dst, dstAlpha) + src;
                }
            }
        }
    }

    void DrawFuncs::DrawRectangle(Surface* target, int2 topLeft, int2 bottomRight, uint col)
    {
        int endX = min(bottomRight.x, target->width),
            endY = min(bottomRight.y, target->height);

        uint srcAlpha = (col & alphaMask) >> 24;
        uint dstAlpha = 255u - srcAlpha;

        uint src = ScaleColor(col, srcAlpha);

        for (int y = max(topLeft.y, 0); y < endY; y++)
        {
            for (int x = max(topLeft.x, 0); x < endX; x++)
            {
                uint& dst = target->pixels[x + y * target->width];
                dst = ScaleColor(dst, dstAlpha) + src;
            }
        }
    }


