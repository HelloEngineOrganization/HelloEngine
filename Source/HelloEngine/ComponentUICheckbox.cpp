#include "Headers.h"
#include "ComponentUICheckbox.h"
#include "GameObject.h"

ComponentUICheckbox::ComponentUICheckbox(GameObject* gameObject) : ComponentUI(gameObject)
{
	State = CheckboxState::NORMAL;
}

ComponentUICheckbox::~ComponentUICheckbox()
{
}

void ComponentUICheckbox::InputUpdate()
{
	// Add here any checks necessary with INPUT.

	// PROBLEMA A SOLUCIONAR: NO PODEM TENIR ACTIVE I HOVERED AL MATEI TEMPS (EN PROCES DE PENSAR UNA SOLUCI�) 


	if (IsMouseOver()) {
		if (ModuleInput::S_GetMouseButton(1) != KEY_DOWN)
		{
			State = CheckboxState::HOVERED;
		}

		if (ModuleInput::S_GetMouseButton(1) == KEY_UP)
		{
			State = CheckboxState::ONPRESS;
		}
	}

	if (State == CheckboxState::ONPRESS)
	{
		if (State != CheckboxState::ACTIVE)
			State = CheckboxState::ACTIVE;

		else
			State = CheckboxState::NORMAL;
	}

	switch (State)
	{
	case CheckboxState::NORMAL:
		Console::S_Log("Im in Disble");
		//LOG("Im in Normal Mode");
		break;
	case CheckboxState::HOVERED:
		Console::S_Log("Im Hovered");
		break;
	case CheckboxState::ONPRESS:
		Console::S_Log("Im get Presed");
		//LOG("Im get Presed");
		break;
	case CheckboxState::ACTIVE:
		Console::S_Log("Im Active");
		//LOG("Im get Presed");
		break;
	default:
		break;
	}

	if (!IsMouseOver())
	{
		State = CheckboxState::NORMAL;
	}
}
