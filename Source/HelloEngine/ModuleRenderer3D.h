#ifndef __MODULERENDERER3D_H__
#define __MODULERENDERER3D_H__

#include "Module.h"
#include "RenderManager.h"

class CameraObject;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	UpdateStatus PreUpdate();
	UpdateStatus PostUpdate();
	bool CleanUp();

	void OnResize(int width, int height);
	void ToggleVSync(bool isOn);

	void ToggleOpenGLSystem(bool enable, int openGLMacro);

	void ToggleOpenGLWireframe(bool enable);

	GameObject* RaycastFromMousePosition(LineSegment& ray, CameraObject* camera);

	void DrawGame();
public:
	RenderManager renderManager;
	SDL_GLContext context;
	bool isVSync = false;

private:
	ModuleCamera3D* _cameras = nullptr;
};

#endif // !__MODULERENDERER3D_H__