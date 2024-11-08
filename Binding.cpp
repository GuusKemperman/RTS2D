#include "precomp.h"
#include "Binding.h"
#include "InputManager.h"
#include "Input.h"
#include "Variable.h"

using namespace std;
using namespace Data;


Binding::Binding(Variable* arg_var, InputManager* inpMngr) : name(arg_var->name), var(arg_var)
{
	stringstream stream(var->value);

	// Convert the space seperated integers of the variable value to a list of integers.
	std::list<string> inputIDs(
		(std::istream_iterator<string>(stream)), // begin
		(std::istream_iterator<string>()));      // end

	for (string str : inputIDs)
	{
		boundTo.push_back(inpMngr->IGetInput(static_cast<InputID>(stoi(str))));
	}
}

bool Binding::IsPressed() const
{
	for (Input* inp : boundTo)
	{
		if (inp->down)
		{
			return true;
		}
	}
	return false;
}

bool Binding::IsHeld() const
{
	for (Input* inp : boundTo)
	{
		if (inp->held)
		{
			return true;
		}
	}
	return false;
}

bool Binding::IsReleased() const
{
	for (Input* inp : boundTo)
	{
		if (inp->up)
		{
			return true;
		}
	}
	return false;
}

//void Binding::BindTo(Input* inp)
//{
//	assert(find(boundTo.begin(), boundTo.end(), inp) == boundTo.end());
//	boundTo.push_back(inp);
//	var->value += ' ' + inp->id;
//}
//
//void Binding::UnbindFrom(Input* arg_inp)
//{
//	list<Input*>::iterator it = find(boundTo.begin(), boundTo.end(), arg_inp);
//
//	assert(it != boundTo.end());
//
//	boundTo.erase(it);
//
//	var->value.clear();
//	for (Input* inp : boundTo)
//	{
//		var->value += ' ' + inp->id;
//	}
//}

