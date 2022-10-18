#pragma once
#include "Module.h"
#include "Layer.h"
#include "GameObject.h"

enum LayersID
{
	GAME,
	UI,
	DEBUG,
	EDITOR,// Editor should always be the last layer
	MAX
};

class ModuleLayers : public Module
{
public:
	ModuleLayers();
	~ModuleLayers();

	bool Init() override;

	bool Start() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	void DrawLayers();

	void DrawEditor();

	bool CleanUp() override;

	uint AddGameObject(GameObject* go);

public:
	Layer* layers[(uint)LayersID::MAX] = { nullptr };

	uint IDcounter = 1;

	GameObject* rootGameObject = nullptr;

	std::map<uint, GameObject*> gameObjects;
	GameObject* go21 = nullptr;
};

