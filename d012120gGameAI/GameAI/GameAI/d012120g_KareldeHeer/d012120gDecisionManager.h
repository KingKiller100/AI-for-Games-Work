#pragma once
#include "../TankManager.h"
#include "../GameObject.h"
#include "../BaseTank.h"
#include "../PickUpManager.h"
#include "d012120gPathFinder.h"
#include "d012120gSteeringBehaviours.h"
#include "../Mine.h"
#include <iostream>
#include <algorithm>

class d012120gDecisionManager
{
public:
	~d012120gDecisionManager();

	void init(BaseTank* myTank);
	static d012120gDecisionManager* Instance();
	void Update(float deltatime);

	Vector2D GetSteeringBehaviour();
	STEERING_BEHAVIOUR GetBehaviour() { return behaviour; }
	Vector2D GetTargetPos() { return targetPos; }
private:
	Vector2D PatrolMovement();
	Vector2D FindPickUp();
	void IsTankStuck();
	Vector2D PatrolEnvironment();
	void AvoidCorners();
	void CheckIfBeingShotBehind();
	
private:
	Vector2D location; // current patrol location

	static d012120gDecisionManager* mInstance;
	vector<GameObject*> mCrates;
	Vector2D targetPos;
	Vector2D mCratePos;
	BaseTank* mTank;
	vector<Vector2D> patrolPath;

	// Tank information to consider for decisions
	int mTankMaxHealth;
	float mTankMaxFuel;
	int mTankMaxWeapons[3]; // element 1 = rockets, element 2 = bullets, element 3 = mines

	vector<BaseTank*> mVisibleEnemyTanks; // List of enemy tanks in front of tank

	STEERING_BEHAVIOUR behaviour;
	d012120gSteeringBehaviours* steeringBeh;

	d012120gSurvivalStates tankSurvivalState;

	double attackTimer = 0;
	const int attackTimeLimit = 5;

	vector<Mine> listOfMines;

	// If tank is stuck in a position for too long //
	double deltaTime;
	double stuckTimer = 0.0;
	double switchLocationTime = 10.5;
	bool isStuck;

	// Stores tank position at a certain time to check if tank has managed to get out of a spot
	Vector2D upperBound;
	Vector2D lowerBound;
};


