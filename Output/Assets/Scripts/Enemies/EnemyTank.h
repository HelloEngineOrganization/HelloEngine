#pragma once
#include "API/HelloBehavior.h"
#include "ScriptToInspectorInterface.h"
#include "Macro.h"

#include "API/API.h"
#include "Enemy.h"
#include "EnemyGun.h"

class EnemyTank : HelloBehavior
{
public:

	enum class States {
		WANDERING,
		TARGETING,
		ATTACKING
	};

	void Recovering();

	void Start() override;
	void Update() override;

	float TakeDamageTank(float life, float damage);
	void CheckDistance();
	void ReturnToZone();
	void MoveToDirection(float pointX, float pointY, float velocity);
	void EscapeFromDirection(float pointX, float pointY, float velocity);

	void Wander();
	void Seek();
	void Attack();

	/*void Seek(float vel, API_Vector3 tarPos, API_RigidBody rb);
	void Attacking(float vel, API_Vector3 tarPos, API_RigidBody rb);*/

	float currentShield;
	float maxShield;
	float recoverShieldTime;

	float healthRestorePerSecond;
	float healthRestoreCooldown;

	//API_GameObject tank;
	Enemy* enemyScript;

	EnemyGun* enemyGun = nullptr;
	API_GameObject gunObj;
	int gunType;
	States state;

	float detectionDistance;
	float walkVelocity;
	float seekVelocity;
	API_Vector3 initialPosition;
	float initPosRange;
	API_GameObject target;

	API_GameObject actionZone;
	API_RigidBody zoneRb;

	float attackRange;
	float separateRange;
	float returnToZoneDistance;

private:
	bool isRecoveringShield;
	bool isRestoringHealth;
	bool hasToRestoreHealth;
	float shieldRecoverCounter;
	float healthRestoreCounter;
	float targetDistance;
	bool isReturning;
};

