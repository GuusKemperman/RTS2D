#include "precomp.h"
#include "MySprite.h"
#include "AssetManager.h"
#include "PlayerData.h"

MySprite::MySprite(shared_ptr<Surface> a_surface, uint a_numOfColumns, uint a_numofRows)
{
	Load(move(a_surface), a_numOfColumns, a_numofRows);
}

MySprite::MySprite(const string& spriteName)
{
	PlayerData data("sprites." + spriteName);

	string surfaceName = data.GetVariableValue("file");
	uint numOfColumns = stoi(data.GetVariableValue("numofcolumns"));
	uint numOfRows = stoi(data.GetVariableValue("numofrows"));

	shared_ptr<Surface> surface = AssetManager::Inst().GetShared(surfaceName);
	Load(move(surface), numOfColumns, numOfRows);
}

MySprite::~MySprite()
{
	surface.reset();
}

MySprite::MySprite(const MySprite& other)
{
	surface = other.surface;
	numOfColumns = other.numOfColumns;
	numOfRows = other.numOfRows;
	frameSize = other.frameSize;
	currentFrame = other.currentFrame;
}

void MySprite::Load(shared_ptr<Surface> a_surface, uint a_numOfColumns, uint a_numOfRows)
{
	surface = move(a_surface);
	numOfColumns = a_numOfColumns;
	numOfRows = a_numOfRows;
	frameSize = { static_cast<uint>(surface->width) / a_numOfColumns, static_cast<uint>(surface->height) / a_numOfRows };
	currentFrame = { 0, 0 };
}

// Used for pixel-collision - Draws on a vector of booleans instead, and returns true if any of the values was already true.
bool MySprite::DrawScaled(vector<bool>& target, const uint& targetWidth, const uint2& position, const uint& scale) const
{
	uint* src = surface->pixels;

	const uint2 srcStart = currentFrame * frameSize;
	const uint2 srcEnd = srcStart + frameSize;

	for (uint srcY = srcStart.y; srcY < srcEnd.y; srcY++, src += surface->width)
	{
		const uint dstStartY = position.y + (srcY - srcStart.y) * scale;
		const uint dstEndY = dstStartY + scale;

		for (uint srcX = srcStart.x; srcX < srcEnd.x; srcX++)
		{
			const uint colSrc = src[srcX];

			if ((colSrc & 0xffffff) == 0)
			{ 
				continue; 
			}

			const uint dstStartX = position.x + (srcX - srcStart.x) * scale;
			const uint dstEndX = dstStartX + scale;
			uint index = dstStartX + dstStartY * targetWidth;

			for (uint y = 0; y < scale; y++, index += targetWidth)
			{
				for (uint x = 0; x < scale; x++, index++)
				{
					// The only part that needs to be different from the normal drawscalled function.
					if (target[index]) 
					{ 
						return true; 
					}
					target[index] = true;
				}
			}
		}
	}

	return false;
}

// The function that I use to draw to the screen.
void MySprite::DrawScaled(Surface* target, const int2& position, const int& scale) const
{
	// If completely offscreen, return.
	if (position.x >= target->width 
		|| position.y >= target->height
		|| position.x + frameSize.x * scale < 0 
		|| position.y + frameSize.y * scale < 0)
	{
		return;
	}

	uint2 srcStart = currentFrame * frameSize;
	uint2 srcEnd = srcStart + frameSize;

	uint* dst = target->pixels;
	uint* src = surface->pixels + srcStart.y * surface->width;

	for (uint srcY = srcStart.y; srcY < srcEnd.y; srcY++, src += surface->width)
	{
		int dstStartY = position.y + (srcY - srcStart.y) * scale;

		if (dstStartY >= target->height)
		{
			break;
		}

		int dstEndY = min(dstStartY + scale, target->height);
		dstStartY = max(dstStartY, 0);

		for (uint srcX = srcStart.x; srcX < srcEnd.x; srcX++)
		{
			const uint colSrc = src[srcX];

			if ((colSrc & 0xffffff) == 0) 
			{ 
				continue; 
			}

			int dstStartX = position.x + (srcX - srcStart.x) * scale;

			if (dstStartX >= target->width)
			{
				break;
			}

			int dstEndX = min(dstStartX + scale, target->width);
			dstStartX = max(dstStartX, 0);

			for (int dstY = dstStartY; dstY < dstEndY; dstY++)
			{
				for (int dstX = dstStartX; dstX < dstEndX; dstX++)
				{
					// Assign the colour to the screen.
					dst[dstX + dstY * target->width] = colSrc;
				}
			}
		}
	}
}

// Function used for pixel collision
bool MySprite::Draw(vector<bool>& target, const uint& targetWidth, const uint2& position) const
{
	uint2 srcStart = currentFrame * frameSize;
	uint* src = surface->pixels + srcStart.x + srcStart.y * surface->width;

	uint2 dstStart = position;
	uint2 dstEnd = position + frameSize;

	uint width = dstEnd.x - dstStart.x;
	uint height = dstEnd.y - dstStart.y;

	uint dstAddr = position.y * targetWidth;
for (uint y = 0; y < height; y++, src += surface->width, dstAddr += targetWidth)
{
	for (uint x = 0; x < width; x++)
	{
		const bool hasColor = src[x] & 0xffffff;

		if (!hasColor)
		{
			continue;
		}

		if (target[dstAddr + x])
		{
			return true;
		}

		target[dstAddr + x] = true;
	}
}
return false;
}

void MySprite::Draw(Surface* target, const int2 position) const
{
	// If completely offscreen, return.
	if (position.x >= target->width
		|| position.y >= target->height
		|| position.x < -static_cast<int>(frameSize.x)
		|| position.y < -static_cast<int>(frameSize.y))
	{
		return;
	}

	uint2 srcStart = currentFrame * frameSize;
	uint2 srcEnd = srcStart + frameSize;

	uint* src = surface->pixels + srcStart.x + srcStart.y * surface->width;

	int2 dstStart = position;
	int2 dstEnd = position + make_int2(frameSize);

	dstEnd.x = min(dstEnd.x, target->width);
	dstEnd.y = min(dstEnd.y, target->height);

	if (dstStart.x < 0)
	{
		src += -dstStart.x;
		dstStart.x = 0;
	}

	if (dstStart.y < 0)
	{
		src += -dstStart.y * surface->width;
		dstStart.y = 0;
	}

	if (dstEnd.x <= dstStart.x
		|| dstEnd.y <= dstStart.y)
	{
		return;
	}

	uint* dst = target->pixels + dstStart.x + dstStart.y * target->width;

	int width = dstEnd.x - dstStart.x;
	int height = dstEnd.y - dstStart.y;

	for (int y = 0; y < height; y++, dst += target->width, src += surface->width)
	{
		for (int x = 0; x < width; x++)
		{
			int line = y + (dstStart.y - position.y);

			const uint colSrc = src[x];
			if (colSrc & 0xffffff)
			{
				dst[x] = colSrc;
			}
		}
	}
}

void MySprite::SetFrame(uint frameIndex)
{
	assert(frameIndex >= 0 && frameIndex < numOfColumns* numOfRows && "Invalid frame given");
	currentFrame.x = frameIndex % numOfColumns, currentFrame.y = frameIndex / numOfColumns;
}

void MySprite::SetFrame(uint2 frame)
{
	assert(frame.x >= 0 && frame.x < numOfColumns&& frame.y >= 0 && frame.y < numOfRows && "Invalid frame given");
	currentFrame = frame;
}

uint MySprite::SamplePixel(uint2 atPosition) const
{
	if (atPosition.x < 0
		|| atPosition.y < 0
		|| atPosition.x >= frameSize.x
		|| atPosition.y >= frameSize.y)
	{
		return 0u;
	}

	uint* pixels = surface->pixels;
	uint2 frameStart = currentFrame * frameSize;
	uint2 samplePoint = frameStart + atPosition;

	return pixels[samplePoint.x + samplePoint.y * surface->width];
}
