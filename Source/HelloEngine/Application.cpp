#include "Headers.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleLayers.h"
#include "ModuleXML.h"
#include "ModuleFiles.h"
#include "ModuleCommand.h"
#include "ModuleResourceManager.h"

#include "API/API_Vector2.h"
#include "API/API_Vector3.h"

using Vector2 = API::API_Vector2;

using Vector3 = API::API_Vector3;

Application* Application::_app = nullptr;

Application::Application()
{
}

Application::~Application()
{
}

bool Application::Init()
{
	float2 f(2, 3);

	Vector2 v2(2, 2);

	v2 = -f;

	Vector3 v3;

	Vector3 v4(5,2,3);

	v3 = -v4;

	v3 = Vector3::S_Identity();

	float result = Vector3::S_Distance(v3, v4);

	//std::cout << v2 << std::endl;

	window = new ModuleWindow(true);
	file = new ModuleFiles();
	input = new ModuleInput(true);
	camera = new ModuleCamera3D(true);
	renderer3D = new ModuleRenderer3D(true);
	layers = new ModuleLayers();
	xml = new ModuleXML();
	command = new ModuleCommand();
	resource = new ModuleResourceManager();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(file);

	AddModule(xml);

	AddModule(window);

	AddModule(input);

	AddModule(resource);

	AddModule(command);

	AddModule(camera);

	AddModule(layers);

	// Renderer last!
	AddModule(renderer3D);

	//Set up frame rate
	XMLNode configNode = xml->GetConfigXML();
	frameCap = configNode.node.child("renderer").child("framerate").attribute("value").as_int(60);
	SetFPS(frameCap);

	// Call Init() in all modules
	for (int i = 0, count = _list_modules.size() ; i <count ; i++)
	{
		_list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");

	for (int i = 0, count = _list_modules.size(); i < count; i++)
	{
		_list_modules[i]->Start();
	}

	return true;
}

void Application::AddModule(Module* mod)
{
	_list_modules.push_back(mod);
}

// Call PreUpdate, Update and PostUpdate on all modules
UpdateStatus Application::Update()
{
	if (_isExiting) 
		return UpdateStatus::UPDATE_STOP;

	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	for (int i = 0, count = _list_modules.size(); i < count && ret == UpdateStatus::UPDATE_CONTINUE; i++)
	{
		ret = _list_modules[i]->PreUpdate();
	}

	for (int i = 0, count = _list_modules.size(); i < count && ret == UpdateStatus::UPDATE_CONTINUE; i++)
	{
		ret = _list_modules[i]->Update();
	}

	for (int i = 0, count = _list_modules.size(); i < count && ret == UpdateStatus::UPDATE_CONTINUE; i++)
	{
		ret = _list_modules[i]->PostUpdate();
	}

	_dt = timer.getDeltaTime();

	if (_dt < fps)
	{
		float sleepTime = (fps - _dt) * 1000;
		Sleep(sleepTime);
	}

	timer.Reset();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	_isExiting = true;

	XMLNode configNode = xml->GetConfigXML();
	configNode.node.child("renderer").child("framerate").attribute("value").set_value(frameCap);

	for (int i = _list_modules.size() -1; i >= 0 && ret; i--)
	{
		ret = _list_modules[i]->CleanUp();
	}

	for (int i = _list_modules.size() - 1; i >= 0; i--)
	{
		RELEASE(_list_modules[i]);
	}

	return ret;
}

Application* Application::Instance()
{
	if (_app == nullptr) 
		_app = new Application();

	return _app;
}

std::string Application::GetEngineVersion()
{
	return "v1.5";
}
