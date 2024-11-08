#pragma once

class Scene;
class Camera;

class Drawable
{
public:
	Drawable(Scene& scene, int priority = 0, bool shouldRegister = true, bool canZoomOn = true);
	virtual ~Drawable();

	virtual void Draw(Surface* target, const Camera* camera) const = 0;

	void SetPriority(int a_priority);
	inline int GetPriority() const { return priority; }

	const bool canZoomOn{};
private:
	Scene& drawToScene;
	int priority{};
};


