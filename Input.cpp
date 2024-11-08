#include "precomp.h"
#include "input.h"

void Input::Down()
{
	if (!held)
	{
		down = true;
	}
	held = true;
}

void Input::Up()
{
	held = false;
	up = true;
}

void Input::ResetForNextFrame()
{
	up = false;
	down = false;
}

