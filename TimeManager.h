#pragma once
#include "Singleton.h"

class TimeManager : 
	Singleton<TimeManager>
{
public:
	static inline float GetDeltaTime() { return Inst().deltaTime; }
	static inline float GetRawDeltaTime() { return Inst().rawDeltaTime; }
	static inline float GetTimeScale() { return Inst().timescale; }
	static inline float GetTotalTimePassed() { return Inst().totalTimePassed; }
	static inline uint GetCurrentFrame() { return Inst().currentFrame; }

	static inline void UpdateDeltaTime(float arg_rawDeltaTime)
	{
		TimeManager& inst = Inst();
		arg_rawDeltaTime /= 1000.0f;
		arg_rawDeltaTime = clamp(arg_rawDeltaTime, 0.0f, inst.maxDeltaTime);
		inst.deltaTime = arg_rawDeltaTime * inst.timescale;
		inst.rawDeltaTime = arg_rawDeltaTime;
		inst.totalTimePassed += inst.deltaTime;

		++inst.currentFrame;
	}

	static inline void SetTimeScale(float scale)
	{
		Inst().timescale = scale;
	}

private:
	float deltaTime{}, rawDeltaTime{}, timescale{ 1.0f };
	float totalTimePassed{};
	const float maxDeltaTime{ 100.0f };
	uint currentFrame{};
};



