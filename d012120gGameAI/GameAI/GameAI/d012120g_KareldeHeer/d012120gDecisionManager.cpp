#include "../d012120g_KareldeHeer/d012120gDecisionManager.h"

// Initalise the instance to null
d012120gDecisionManager* d012120gDecisionManager::mInstance = nullptr;

void d012120gDecisionManager::init(BaseTank * myTank)
{
	isStuck = false;
	steeringBeh = new d012120gSteeringBehaviours(myTank);
	mTank = myTank;
	mTankMaxFuel = myTank->GetFuel();
	mTankMaxHealth = myTank->GetHealth();
	mTankMaxWeapons[WEAPONTYPE_ROCKETS] = myTank->GetRockets();
	mTankMaxWeapons[WEAPONTYPE_BULLETS] = myTank->GetBullets();
	mTankMaxWeapons[WEAPONTYPE_MINES] = myTank->GetMines();

	mVisibleEnemyTanks = TankManager::Instance()->GetVisibleTanks(mTank);

	patrolPath.push_back(Vector2D(150, 555));
	patrolPath.push_back(Vector2D(750, 100));
	patrolPath.push_back(Vector2D(825, 555));
	patrolPath.push_back(Vector2D(150, 555));

	Vector2D getCurrentTankPos = mTank->GetCentralPosition();

	vector<Vector2D>::iterator nearestPoint = std::min_element(patrolPath.begin(), patrolPath.end(), [getCurrentTankPos](auto&a, auto&b) {return a.Distance(getCurrentTankPos) < b.Distance(getCurrentTankPos); });
	targetPos = *nearestPoint;
	location = targetPos;

	tankSurvivalState = TANK_PATROLSTATE;
}

d012120gDecisionManager * d012120gDecisionManager::Instance()
{
	if (!mInstance)
		mInstance = new d012120gDecisionManager;

	return mInstance;
}

d012120gDecisionManager::~d012120gDecisionManager()
{
	mInstance = nullptr;
}

void d012120gDecisionManager::Update(float deltatime)
{
	mVisibleEnemyTanks = TankManager::Instance()->GetVisibleTanks(mTank);
	this->deltaTime = deltatime;
	mCrates = PickUpManager::Instance()->GetAllPickUps();

	if (mTank->GetHealth() <= mTankMaxHealth * 0.5 && mTank->GetHealth() >= mTankMaxHealth * 0.25 && mVisibleEnemyTanks.size() > 1)
		tankSurvivalState = TANK_FLEESTATE;
	else if (mTank->GetHealth() > mTankMaxHealth * 0.5 && (mVisibleEnemyTanks.size() > 0))
		tankSurvivalState = TANK_ATTACKSTATE;
	else if (mTank->GetHealth() < mTankMaxHealth * 0.25 && mTank->GetBullets() < mTankMaxWeapons[WEAPONTYPE_BULLETS] * 0.15 && mVisibleEnemyTanks.size() > 3)
		tankSurvivalState = Tank_EVADESTATE;
	else
		tankSurvivalState = TANK_PATROLSTATE;

	if (tankSurvivalState == TANK_PATROLSTATE)
		targetPos = PatrolEnvironment();
	else if (tankSurvivalState == TANK_ATTACKSTATE)
	{
		stuckTimer = 0;
		attackTimer += deltaTime;
		if (mTank->GetBullets() > mTankMaxWeapons[WEAPONTYPE_BULLETS] * 0.2)
		{
			if (mTank->GetCentralPosition().Distance(mVisibleEnemyTanks[0]->GetCentralPosition()) < 60)
				behaviour = STEERING_PURSUIT;
			mTank->ChangeState(TANKSTATE_MANFIRE);
		}
		if (mTank->GetRockets() > mTankMaxWeapons[WEAPONTYPE_ROCKETS] * 0.2)
		{
			if (mTank->GetCentralPosition().Distance(mVisibleEnemyTanks[0]->GetCentralPosition()) < 150)
				behaviour = STEERING_PURSUIT;
		}
		mTank->ChangeState(TANKSTATE_IDLE);

		if (attackTimer > attackTimeLimit)
		{
			tankSurvivalState = TANK_FLEESTATE;
			attackTimer = 0;
		}
	}
	else if (tankSurvivalState == TANK_FLEESTATE)
	{
		attackTimer = 0;
		behaviour = STEERING_FLEE;
	}
	else if (tankSurvivalState == Tank_EVADESTATE)
	{
		behaviour = STEERING_EVADE;
	}

	AvoidCorners();
}


Vector2D d012120gDecisionManager::PatrolEnvironment()
{
	if (mCrates.size() > 0)
		if (!isStuck)
			return FindPickUp();
	else
		stuckTimer = 0;

	return PatrolMovement();
}

void d012120gDecisionManager::AvoidCorners()
{
	if (mTank->GetCentralPosition().Distance(Vector2D(0.0, 0.0)) < 120) // avoids top left corner
	{
		tankSurvivalState == TANK_FLEESTATE;
		mTank->ChangeState(TANKSTATE_DROPMINE); // when close to a pickup, tank drops a mine near it's location
		mCratePos = Vector2D(0.0, 0.0);
	}
	else if (mTank->GetCentralPosition().Distance(Vector2D(kScreenWidth, kScreenHeight)) < 120) // avoids bottom right corner
	{
		tankSurvivalState == TANK_FLEESTATE;
		mTank->ChangeState(TANKSTATE_DROPMINE); // when close to a pickup, tank drops a mine near it's location
		mCratePos = Vector2D(kScreenWidth, kScreenHeight);
	}
	else if (mTank->GetCentralPosition().Distance(Vector2D(0.0, kScreenHeight)) < 120) // avoids left right corner
	{
		tankSurvivalState == TANK_FLEESTATE;
		mTank->ChangeState(TANKSTATE_DROPMINE); // when close to a pickup, tank drops a mine near it's location
		mCratePos = Vector2D(0.0, kScreenHeight);
	}
	else if (mTank->GetCentralPosition().Distance(Vector2D(kScreenWidth, 0.0)) < 120) // avoids  top right corner
	{
		tankSurvivalState == TANK_FLEESTATE;
		mTank->ChangeState(TANKSTATE_DROPMINE); // when close to a pickup, tank drops a mine near it's location
		mCratePos = Vector2D(kScreenWidth, 0.0);
	}
}

void d012120gDecisionManager::CheckIfBeingShotBehind()
{

}

Vector2D d012120gDecisionManager::FindPickUp()
{
	behaviour = STEERING_ARRIVE;

	if (mTank->GetCentralPosition().Distance(mCrates[0]->GetCentralPosition()) < 40)
		mTank->ChangeState(TANKSTATE_DROPMINE); // when close to a pickup, tank drops a mine near it's location

	mCratePos = mCrates[0]->GetCentralPosition();
	
	IsTankStuck();

	return mCratePos;
}

Vector2D d012120gDecisionManager::PatrolMovement()
{
	behaviour = STEERING_SEEK;

	if (isStuck)
	{
		Vector2D getCurrentTankPos = mTank->GetCentralPosition();

		vector<Vector2D>::iterator nearestPoint = std::min_element(patrolPath.begin(), patrolPath.end(), [getCurrentTankPos](auto&a, auto&b) {return a.Distance(getCurrentTankPos) < b.Distance(getCurrentTankPos); });
		targetPos = *nearestPoint;
		location = targetPos;

		isStuck = false;
	}

	if (mTank->GetCentralPosition().Distance(Vector2D(150, 100)) < 50)
	{
		mTank->ChangeState(TANKSTATE_DROPMINE);
		location = Vector2D(150, 555);
	}

	if (mTank->GetCentralPosition().Distance(Vector2D(150, 555)) < 50)
	{
		mTank->ChangeState(TANKSTATE_DROPMINE);
		location = Vector2D(750, 100);
	}

	if (mTank->GetCentralPosition().Distance(Vector2D(750, 100)) < 50)
	{
		mTank->ChangeState(TANKSTATE_DROPMINE);
		location = Vector2D(825, 555);
	}

	if (mTank->GetCentralPosition().Distance(Vector2D(825, 555)) < 50)
	{
		mTank->ChangeState(TANKSTATE_DROPMINE);
		location = Vector2D(kScreenWidth / 2, kScreenHeight / 2);
	}

	if (mTank->GetCentralPosition().Distance(Vector2D(kScreenWidth / 2, kScreenHeight / 2)) < 50)
	{
		mTank->ChangeState(TANKSTATE_DROPMINE);
		location = Vector2D(150, 100);
	}

	return location;
}

void d012120gDecisionManager::IsTankStuck()
{
	stuckTimer += deltaTime;

	if (stuckTimer > switchLocationTime && stuckTimer < switchLocationTime + 1.5)
	{
		upperBound = mTank->GetCentralPosition() + Vector2D(20, 20);
		lowerBound = mTank->GetCentralPosition() - Vector2D(20, 20);
	}

	if (stuckTimer > switchLocationTime * 1.5 && stuckTimer < switchLocationTime * 1.5 + 1.5)
	{
		if ((mTank->GetCentralPosition().x < upperBound.x && mTank->GetCentralPosition().x > lowerBound.x) && (mTank->GetCentralPosition().y < upperBound.y && mTank->GetCentralPosition().y > lowerBound.y))
		{
			mCratePos = Vector2D(rand() % kScreenWidth, rand() % kScreenHeight);
			mTank->ChangeState(TANKSTATE_DROPMINE);
			isStuck = true;
		}
			stuckTimer = 0;
	}
}

Vector2D d012120gDecisionManager::GetSteeringBehaviour()
{
	if (behaviour == STEERING_ARRIVE)
			return steeringBeh->Arrive(targetPos, fast) + steeringBeh->ObstacleAvoidance();

	if (behaviour == STEERING_SEEK)
			return steeringBeh->Seek(targetPos) + steeringBeh->ObstacleAvoidance();

	if (behaviour == STEERING_FLEE)
		return steeringBeh->ProximityFlee(targetPos) + steeringBeh->ObstacleAvoidance();

	if (behaviour == STEERING_PURSUIT)
		for (auto enemyTank : mVisibleEnemyTanks)
			return steeringBeh->Pursuit(enemyTank) + steeringBeh->ObstacleAvoidance();

	if (behaviour == STEERING_EVADE)
		for (auto enemyTank : mVisibleEnemyTanks)
			return steeringBeh->Evade(enemyTank) + steeringBeh->ObstacleAvoidance();
}