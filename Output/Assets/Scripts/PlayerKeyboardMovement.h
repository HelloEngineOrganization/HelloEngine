#pragma once
#include "API/HelloBehavior.h"
#include "ScriptToInspectorInterface.h"
#include "Macro.h"

#include "API/API.h"

class PlayerKeyboardMovement : HelloBehavior
{
public:
	void Start() override; 
	void Update() override;

	void MouseAim();

	void Dash(float dt);

	//player velocity on keyboard
	float velk = 1.0f;

	float maxVel = 0.15f;
	float accel = 0.15f;
	float brake = 0.4f;
	bool moving = false;
	bool braking = false;
	bool movF = false;
	bool movB = false;
	bool movR = false;
	bool movL = false;

	float startDash = 0.0f;
	float endDash = 0.05f;
	float dashCooldown = 5.0f;
	bool dashActive = false;
	bool dashEnable = true;

	API_Transform cam;
	API_GameObject finalCam;
	API_Transform MainCam;
	API_Transform StarShipCam;

private:
	float _angle;

};

