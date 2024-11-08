#include "precomp.h"
#include "DynamicCursor.h"
#include "MySprite.h"
#include "TimeManager.h"

DynamicCursor::DynamicCursor(Scene& scene) :
	UISprite(scene, "cursor")
{

}

DynamicCursor::~DynamicCursor()
{
}

void DynamicCursor::Draw(Surface* target, const Camera* camera) const
{
	if (currentState == State::invisible)
	{
		return;
	}

	float timePassed = TimeManager::GetTotalTimePassed();
	uint animationOffset = static_cast<uint>(fmodf(timePassed, static_cast<float>(sprite->GetNumberOfColumns()) / static_cast<float>(animFps)) * animFps);
	uint2 frame(animationOffset, static_cast<uint>(currentState));
	sprite->SetFrame(frame);

	int2 centreOffset = make_int2((sprite->GetFrameSize() * scale) / 2);

	sprite->DrawScaled(target, screenPos - centreOffset, scale);
}

void DynamicCursor::SetState(State to)
{
	uint row = static_cast<uint>(to);
	assert(row >= 0 && row < sprite->GetNumberOfRows());
	currentState = to;
}

void DynamicCursor::SetScreenPosition(int2 a_screenPos)
{
	screenPos = a_screenPos;
}
