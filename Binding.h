#pragma once

class Input;
class InputManager;

namespace Data
{
	class Variable;
}

class Binding
{
public:
	Binding() = default;
	Binding(Data::Variable* var, InputManager* inpMngr);

	bool IsPressed() const;
	bool IsHeld() const;
	bool IsReleased() const;

	//void BindTo(Input* inp);
	//void UnbindFrom(Input* inp);

	const std::string name;

private:
	std::list<Input*> boundTo;
	Data::Variable* var{};
};



