#pragma once
#include "DataSuper.h"

class Action :
	public DataSuper<Action>
{
public:
	Action(const string& actionType);

	const string type{};
	uint index{};
	uint frame{};
};

