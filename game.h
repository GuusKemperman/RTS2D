#pragma once

class Scene;

namespace Tmpl8
{

class Game : public TheApp
{
public:
	Game();
	~Game();

	// game flow methods
	void Init();
	bool Tick( float deltaTime );
	void Shutdown() { /* implement if you want to do something on exit */ }
	// input handling
	void MouseUp(int button);
	void MouseDown(int button);
	void MouseMove(int x, int y);
	void MouseWheel(float y);
	void KeyUp(int key);
	void KeyDown(int key);

	void RequestLoadTo(unique_ptr<Scene> scene);
private:
	void SwitchToRequestedScene();

	unique_ptr<Scene> activeScene{};
	unique_ptr<Scene> requestLoadTo{};
	bool gameRunning = true;
};

} // namespace Tmpl8