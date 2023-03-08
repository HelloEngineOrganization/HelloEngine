#include "PlayerGamepadMovement.h"
#include "SwapCam.h"

HELLO_ENGINE_API_C PlayerGamepadMovement* CreatePlayerGamepadMovement(ScriptToInspectorInterface* script)
{
	PlayerGamepadMovement* classInstance = new PlayerGamepadMovement();
	//Show variables inside the inspector using script->AddDragInt("variableName", &classInstance->variable);

	script->AddDragFloat("Velocity", &classInstance->vel);
	script->AddDragFloat("Max Velocity", &classInstance->maxVel);
	script->AddDragFloat("Acceleration", &classInstance->accel);
	script->AddDragFloat("Brake", &classInstance->brake);
	script->AddDragBoxGameObject("Aux Cam", &classInstance->finalCam);
	script->AddDragBoxTransform("Camera player", &classInstance->MainCam);
	script->AddDragBoxTransform("Camera Starship", &classInstance->StarShipCam);

	return classInstance;
}

void PlayerGamepadMovement::Start()
{
	_angle = 0.0f;
	vel = 0.0f;
}
void PlayerGamepadMovement::Update()
{

	SwapCam* cameraScript = (SwapCam*)finalCam.GetScript("SwapCam");

	if (cameraScript != nullptr) {
		if (cameraScript->mainCamActive) {
			cam = MainCam;
		}
		else {
			cam = StarShipCam;
		}
	}

	float dt = Time::GetDeltaTime();

	//RIGHT AXIS
	GamepadAim();

	//LEFT AXIS

	API_Vector2 movDir;

	movDir.x = Input::GetGamePadAxis(GamePadAxis::AXIS_LEFTX);
	movDir.y = Input::GetGamePadAxis(GamePadAxis::AXIS_LEFTY);

	API_Vector2 normMovDir;
	normMovDir.x = movDir.x / sqrt(pow(movDir.x, 2) + pow(movDir.y, 2));
	normMovDir.y = movDir.y / sqrt(pow(movDir.x, 2) + pow(movDir.y, 2));

	if ((movDir.x > 10000 || movDir.x < -10000 || movDir.y > 10000 || movDir.y < -10000))
	{

		if (vel < maxVel) {
			vel += accel * dt;
		}

		moving = true;
		braking = false;
		//gameObject.GetTransform().Translate(-1.0f * normMovDir.x * vel, 0, -1.0f * normMovDir.y * vel);

	}
	else
	{
		if (vel > 0 )
		{
			vel -= brake * dt;
			//gameObject.GetTransform().Translate(gameObject.GetTransform().GetGlobalPosition().x * vel,0, gameObject.GetTransform().GetGlobalPosition().z * vel);
			// moving = false;
			braking = true;
		}
	}

	if (vel <= 0) {
		moving = false;
		braking = false;
		brake = 0.05f;
		vel = 0.0f;
		movB = movF = movR = movL = false;
	
	}

	//DASH
	if (Input::GetGamePadButton(GamePadButton::BUTTON_X) == KeyState::KEY_DOWN && dashActive == false) {
		dashActive = true;
	}
	if (dashActive) {
		Dash(dt);
	}
	else {
		dashCooldown -= dt;
		if (dashCooldown <= 0) {
			dashCooldown = 5.0f;
		}
	}

	if (movDir.x > 10000)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetLeft() * vel);
		movR = true;
		//gameObject.GetTransform().Translate(-0.05f, 0, 0);

	}
	else if (movR && braking)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetLeft() * vel);
		//Console::Log("BRAKING 1");
		//if (vel <= 0) movR =false;
	}

	if (movDir.x < -10000)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetRight() * vel);
		//gameObject.GetTransform().Translate(0.05f, 0, 0);
		movL = true;
	}
	else if (movL && braking)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetRight() * vel);
		//Console::Log("BRAKING 2");
		//if (vel <= 0) movL = false;
	}

	if (movDir.y > 10000)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetBackward() * vel);
		//gameObject.GetTransform().Translate(0, 0, -0.05f);
		movF = true;
	}
	else if (movF && braking)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetBackward() * vel);
		//Console::Log("BRAKING 3");
		//if (vel <= 0) movF = false;
	}

	if (movDir.y < -10000)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetForward() * vel);
		//gameObject.GetTransform().Translate(0, 0, 0.05f);
		movB = true;
	}
	else if (movB && braking)
	{
		gameObject.GetTransform().Translate(cam.GetGameObject().GetTransform().GetForward() * vel);
		//Console::Log("BRAKING 4");
		//if (vel <= 0) movB = false;
	
	}


	//Console::Log(std::to_string());


	//AIM TO TOP
	if (Input::GetGamePadButton(GamePadButton::BUTTON_UP) == KeyState::KEY_REPEAT /*||Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTY) < -10000*/)
	{
		gameObject.GetTransform().SetRotation(0, 0, 0);

	}
	//AIM TO RIGHT
	if (Input::GetGamePadButton(GamePadButton::BUTTON_RIGHT) == KeyState::KEY_REPEAT /*|| Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTX) > 10000*/)
	{
		gameObject.GetTransform().SetRotation(0, 270, 0);

	}
	//AIM TO DOWN
	if (Input::GetGamePadButton(GamePadButton::BUTTON_DOWN) == KeyState::KEY_REPEAT /*|| Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTY) > 10000*/ )
	{
		gameObject.GetTransform().SetRotation(0, 180, 0);

	}
	//AIM TO LEFT
	if (Input::GetGamePadButton(GamePadButton::BUTTON_LEFT) == KeyState::KEY_REPEAT/* || Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTX) < -10000*/)
	{
		gameObject.GetTransform().SetRotation(0, 90, 0);

	}

	if (Input::GetGamePadButton(GamePadButton::BUTTON_A) == KeyState::KEY_DOWN)
	{
		gameObject.GetTransform().SetPosition({ 0,0,0 });
	}

	if (Input::GetGamePadButton(GamePadButton::BUTTON_X) == KeyState::KEY_DOWN)
	{
		int a = gameObject.GetTransform().GetGlobalRotation().y;
		
		Console::Log(std::to_string(a));
	}

	
	
}


void PlayerGamepadMovement::Dash(float dt)
{
	startDash += dt;
	if (startDash <= endDash) {
		accel = 200.0f;
	}
	else {
		accel = 0.15f;
		vel = 0.0f;
		startDash = 0.0f;
		dashActive = false;
	}
}

void PlayerGamepadMovement::GamepadAim()
{

	API_Vector2 mousePos;
	
	mousePos.x = Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTX);
	mousePos.y = -Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTY);


	API_Vector2 playerPos ;
	playerPos.x = gameObject.GetTransform().GetGlobalPosition().x;
	playerPos.y = gameObject.GetTransform().GetGlobalPosition().y;
	API_Vector2 lookDir;
	
	lookDir.x = (mousePos.x );
	lookDir.y = (mousePos.y );
	API_Vector2 normLookDir ;
	normLookDir.x = lookDir.x / sqrt(pow(lookDir.x, 2) + pow(lookDir.y, 2));
	normLookDir.y = lookDir.y / sqrt(pow(lookDir.x, 2) + pow(lookDir.y, 2));

	if (Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTX) > 10000 || Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTX) < -10000
		|| Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTY) > 10000 || Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTY) < -10000)
	{
	 _angle = atan2(normLookDir.y, normLookDir.x) * RADTODEG - 90.0f;
	}
	

	if (Input::GetKey(KeyCode::KEY_F) == KeyState::KEY_DOWN)
	{
		Console::Log(std::to_string(_angle));

		Console::Log(std::to_string(Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTX)));
		Console::Log(std::to_string(Input::GetGamePadAxis(GamePadAxis::AXIS_RIGHTY)));

		Console::Log(std::to_string(mousePos.x));
		Console::Log(std::to_string(mousePos.y));


	}
	gameObject.GetTransform().SetRotation(0, _angle, 0);

}
