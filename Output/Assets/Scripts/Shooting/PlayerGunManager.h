#pragma once
#include "API/HelloBehavior.h"
#include "ScriptToInspectorInterface.h"
#include "Macro.h"
#include <vector>

#include "API/API.h"

#include "PlayerGun.h"
#include "PlayerGunType.h"
#include "../Player/PlayerStats.h"

class SwapWeapon;
class PlayerMove;

class PlayerGunManager : HelloBehavior
{
public:
    void Start() override;
    void Update() override;

    void GetGun(int slot, int gunIndex);
    void EquipGun(int index);

    API_GameObject player;
    PlayerStats* playerStats;

    std::vector<API_GameObject> guns;
    int equipedIndex;
    PlayerGun* equipedGun = nullptr;
    int gunOnHandIndex1;
    int gunOnHandIndex2;
    int gunOnHandIndex3;

    bool canShoot = true;
    float bufferRB;
    float bufferLB;

    // guns
    API_GameObject duals; // index 0
    API_GameObject semiauto; // index 1
    API_GameObject automatic; // index 2
    API_GameObject burst; // index 3
    API_GameObject shotgun; // index 4
    API_GameObject handgun; // index 5
    API_GameObject flamethrower; // index 6
    API_GameObject ricochet; // index 7

     // HUD
    API_GameObject HUDGameObject;
    SwapWeapon* HUDScript;

    // Player reference
    API_GameObject playerMoveGameObject;
    PlayerMove* playerMovementScript = nullptr;
};