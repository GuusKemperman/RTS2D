#include "precomp.h"
#include "InputManager.h"
#include "Input.h"
#include "Binding.h"
#include "PlayerData.h"

InputManager::InputManager() : data{ new PlayerData("input") }
{
	for (auto& inp : inputs)
	{
		inp = make_unique<Input>();
	}
	// Make sure every bound input also has a default value.
	assert(data->GetVariables().size() == data->GetVariables("default").size());

	LoadKeyBindingsFromFile();
}

InputManager::~InputManager() {}

void InputManager::Tick()
{
	while (!interactedWith.empty())
	{
		Input* inp = interactedWith.front();
		interactedWith.pop();

		// ResetTimeAlive the input state for the next frame.
		inp->ResetForNextFrame();
	}

	mouseWheelChange = 0;
}

const Binding& InputManager::GetBinding(std::string_view name)
{
	return *Inst().bindingsByName.at(name);
}

void InputManager::LoadKeyBindingsFromFile(bool loadDefaultValues)
{
	bindings.clear();
	bindingsByName.clear();

	list<Data::Variable*> inputSettings = data->GetVariables((loadDefaultValues ? "default" : ""));
	for (Data::Variable* var : inputSettings)
	{
		auto kb = make_unique<Binding>(var, this);
		bindingsByName[kb->name] = kb.get();
		bindings.push_back(move(kb));
	}
}

//void InputManager::Unbind(std::string_view bindingName, InputID fromInput)
//{
//	InputManager& instance = Inst();
//	instance.bindingsByName.at(bindingName)->UnbindFrom(&instance.inputs.at(fromInput));
//}
//
//void InputManager::Bind(std::string_view bindingName, InputID toInput)
//{
//	InputManager& instance = Inst();
//	instance.bindingsByName.at(bindingName)->BindTo(&instance.inputs.at(toInput));
//}
//
//void InputManager::BindToNextInput(std::string_view bindingName)
//{
//	InputManager& instance = Inst();
//	instance.currentlyRebinding = instance.bindingsByName.at(bindingName);
//}

void InputManager::KeyDown(int key)
{
	UpdateInputState(ToInputID(key, glfwType::key), InputManager::Interaction::pressed);
}

void InputManager::KeyUp(int key)
{
	UpdateInputState(ToInputID(key, glfwType::key), Interaction::released);
}

void InputManager::MouseDown(int button)
{
	UpdateInputState(ToInputID(button, glfwType::mousebutton), Interaction::pressed);
}

void InputManager::MouseUp(int button)
{
	UpdateInputState(ToInputID(button, glfwType::mousebutton), Interaction::released);
}

void InputManager::MouseWheel(float f)
{
	mouseWheelChange = static_cast<int>(f);
}

void InputManager::MouseMove(int x, int y)
{
	mousePos = { x, y };
}

InputID InputManager::ToInputID(uint num, glfwType type)
{
	switch (type)
	{
	case InputManager::glfwType::key:
		return num;
		break;
	case InputManager::glfwType::mousebutton:
		return num + GLFW_KEY_LAST;
		break;
	default:
		static_assert("glfwType not supported");
		break;
	}
	return InputID();
}

void InputManager::UpdateInputState(InputID keyCode, Interaction interaction)
{
	Input* inp = inputs[keyCode].get();

	//if (currentlyRebinding && interaction == Interaction::pressed)
	//{
	//	currentlyRebinding->BindTo(inp);
	//	currentlyRebinding = nullptr;
	//}

	interactedWith.push(inp);

	if (interaction == Interaction::pressed)
	{
		inp->Down();
		LOGMESSAGE(keyCode << " pressed.");
	}
	else
	{
		inp->Up();
	}
}



