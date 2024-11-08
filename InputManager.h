#pragma once
#include "Singleton.h"

class PlayerData;
class Input;
class Binding;

constexpr size_t supportedInputCount = GLFW_KEY_LAST + GLFW_MOUSE_BUTTON_LAST;

// The index of an Input class.
using InputID = size_t;

class InputManager : 
	public Singleton<InputManager>
{
	friend Singleton;
	InputManager();
	~InputManager();
public:
	void Tick();
	static const Binding& GetBinding(std::string_view name);

	enum class glfwType { key, mousebutton };
	static InputID ToInputID(uint glfwNum, glfwType type);

	static inline int2 GetMousePos() { return Inst().mousePos; };
	static inline int GetMouseWheel() { return Inst().mouseWheelChange; }

	static inline const Input& GetInput(InputID inputID) { return *Inst().IGetInput(inputID); }
	template <typename T> static inline Input GetInput(T notAnID) = delete;

	inline Input* IGetInput(InputID inputID) { return inputs[inputID].get(); }
	template <typename T> inline Input* IGetInput(T notAnID) = delete;

	void LoadKeyBindingsFromFile(bool loadDefaultValues = false);

	//void Unbind(std::string_view bindingName, InputID fromInput);
	//template <typename T> void Unbind(std::string_view bindingName, T notAnID) = delete;

	//void Bind(std::string_view bindingName, InputID toInput);
	//template <typename T> void Bind(std::string_view bindingName, T notAnID) = delete;

	//// The next input given will be bound to the given keybinding.
	//void BindToNextInput(std::string_view bindingName);

	void KeyDown(int key);
	void KeyUp(int key);
	void MouseDown(int button);
	void MouseUp(int button);

	void MouseWheel(float f);
	void MouseMove(int x, int y);
private:
	enum class Interaction { pressed, released };
	void UpdateInputState(InputID keyCode, Interaction interaction);
	template <typename T> void UpdateInputState(T notAnID, Interaction interaction) = delete;


	int2 mousePos{};
	int mouseWheelChange{};

	PlayerData* data{};
	std::list<unique_ptr<Binding>> bindings;
	std::unordered_map<std::string_view, Binding*> bindingsByName{};

	// All the inputs that were pressed or released this frame, and which were bound to atleast one keybinding. Can be used to update the pressed and released state.
	std::queue<Input*> interactedWith{};
	std::array<unique_ptr<Input>, supportedInputCount> inputs{};

	//Binding* currentlyRebinding = nullptr;
};


