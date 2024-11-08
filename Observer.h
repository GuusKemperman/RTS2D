#pragma once
class Observer
{
public:
	virtual ~Observer() {}

	virtual void ObservationMade() = 0;
};

