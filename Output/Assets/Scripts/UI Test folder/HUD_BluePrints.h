#pragma once
#include "API/HelloBehavior.h"
#include "ScriptToInspectorInterface.h"
#include "Macro.h"
#include "API/API_UIText.h"
#include "API/API.h"

#include "../InteractiveEnviroment/ChestManager.h"
class HUD_BluePrints : HelloBehavior
{
public:
	void Start() override; 
	void Update() override;

	void UpgradeAlert();
	void New_WeaponAlert(int GetNewWeapon);
	void Special_WeaponAlert(int GetSpwcialWeapon);

	API_Vector3 upgrade_movmentX;
	API_Vector3 new_weapon_movmentX;
	API_Vector3 special_weapon_movmentX;

	API_Material material_upgrade;
	API_Material material_new_weapon;
	API_Material material_special_weapon;
	
	API_UIText text_upgrade;
	API_UIText text_new_weapon;
	API_UIText text_special_weapon;

	float timer_upgrade = 1.3f;
	float timer_new = 1.3f;
	float timer_special = 1.3f;

	uint upgrade_textures[3];
	uint new_weapon_textures[5];
	uint special_weapon_textures[2];

	API_Vector3 upgrade_position_save;
	API_Vector3 new_weapon_position_save;
	API_Vector3 special_weapon_position_save;
	API_Vector3 text_upgrade_position_save;
	API_Vector3 text_new_weapon_position_save;
	API_Vector3 text_special_weapon_position_save;

	ChestManager* chest_items;
};

