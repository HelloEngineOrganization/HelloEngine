#pragma once
#include "API/HelloBehavior.h"
#include "ScriptToInspectorInterface.h"
#include "Macro.h"
#include "../Player/PlayerStats.h"

#include "API/API.h"

class ThanosLoop : HelloBehavior
{
public:
    void Start() override;
    void Update() override;

    void OnCollisionEnter(API::API_RigidBody other);

    void TakeDamage(float damage);

    float hp = 1000.0f;

    float meleeDmg = 20.0f;

    // bomb mechanic
    void AddBomb();
    void CheckBombs();
    int maxBombNum = 6;
    int currentBombNum = 0;
    API_GameObject bomb;
    uint textureBomb[6];

    // burn
    void AddBurn();
    float burnTime = 0.0f;
    float resetBurn;

};

