#include "d012120gTank.h"
#include "../TankManager.h"
#include "../Collisions.h"
#include "../C2DMatrix.h"

//--------------------------------------------------------------------------------------------------

d012120gTank::d012120gTank(SDL_Renderer* renderer, TankSetupDetails details)
	: BaseTank(renderer, details)
{
	d012120gDecisionManager::Instance()->init(this);

	mCurrentSpeed = 0.0;

	angle = 0.0;
}

//--------------------------------------------------------------------------------------------------

d012120gTank::~d012120gTank()
{
}

//--------------------------------------------------------------------------------------------------

void d012120gTank::ChangeState(BASE_TANK_STATE newState)
{
	BaseTank::ChangeState(newState);
}

//--------------------------------------------------------------------------------------------------

void d012120gTank::Update(float deltaTime, SDL_Event e)
{
	d012120gDecisionManager::Instance()->Update(deltaTime);

	RotateToFacePosition(deltaTime);

	for (auto hearableTanks : mTanksICanHear)
	{
		if (GetCentralPosition().Distance(hearableTanks->GetCentralPosition()) < 10) 
		ChangeState(TANKSTATE_DROPMINE);
	}

	ShowCursor(false);
	ShowCursor(true);

	//Call parent update.
	BaseTank::Update(deltaTime, e);
}


void d012120gTank::RotateToFacePosition(float deltaTime)
{
	if (mTanksICanSee.size() > 0)
	{
		Vector2D targetEnemy = mTanksICanSee[0]->GetCentralPosition() - GetCentralPosition();
		targetEnemy.Normalize();

		double dot = targetEnemy.Dot(mManFireDirection);

		double targetAngle = atan2(targetEnemy.x, targetEnemy.y);
		double manAngle = atan2(mManFireDirection.x, mManFireDirection.y);
		float deltaAngle = targetAngle - manAngle;

		if (sin(deltaAngle) > 0)
			RotateManByRadian(kManTurnRate, -1, deltaTime);
		else
			RotateManByRadian(kManTurnRate, 1, deltaTime);

		if (dot < 0.9)
			FireABullet();

		if (GetCentralPosition().Distance(mTanksICanSee[0]->GetCentralPosition()) < 100)
		{
			Vector2D enemyPos = Vector2D();

			if (d012120gDecisionManager::Instance()->GetBehaviour() == STEERING_PURSUIT)
				enemyPos = d012120gDecisionManager::Instance()->GetSteeringBehaviour();

			enemyPos.Normalize();

			double tanHeading = atan2(GetHeading().y, GetHeading().x);
			double tanTarget = atan2(enemyPos.y, enemyPos.x);

			double angle = tanTarget - tanHeading;

			double sign;

			if (tanTarget > tanHeading)
				sign = -1 * TwoPi;
			else
				sign = TwoPi;

			if (abs(sign + angle) < abs(angle))
				angle += sign;

			angle *= deltaTime;

			if (mVelocity.Length() > GetMaxSpeed() * 0.5)
				mVelocity = mVelocity * 0.15;

			FireRockets();
			BaseTank::RotateHeadingByRadian(angle, sign, deltaTime);
		}
	}
}

void d012120gTank::Render()
{
	BaseTank::Render();

	// Draws mouse click position
	DrawDebugCircle(d012120gDecisionManager::Instance()->GetTargetPos(), 10, 200, 164, 5);
}

//--------------------------------------------------------------------------------------------------

void d012120gTank::MoveInHeadingDirection(float deltaTime)
{
	Vector2D visibleDistance = mVelocity;
	// Rotates tank body to direction towards/away from the target
	if (mVelocity.Length() == 0.0)
	{
		mVelocity = mHeading;
	}
	Vector2D rotateToTarget = GetCentralPosition() + mVelocity;
	RotateHeadingToFacePosition(rotateToTarget, deltaTime);

	force = d012120gDecisionManager::Instance()->GetSteeringBehaviour();

	//Acceleration = Force/Mass
	Vector2D acceleration = force / GetMass();

	//Update velocity.
	mVelocity += acceleration * deltaTime;

	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed()); //TODO: Add Penalty for going faster than MAX Speed.

	mCurrentSpeed = mVelocity.Length();

	//Finally, update the position.
	Vector2D newPosition = GetPosition();
	newPosition.x += mVelocity.x * deltaTime;
	newPosition.y += mVelocity.y * deltaTime;	//Y flipped as adding to Y moves down screen.
	SetPosition(newPosition);
}

void d012120gTank::RotateHeadingByRadian(double radian, int sign)
{
	//Clamp the amount to turn to the max turn rate.
	if (radian > mMaxTurnRate)
		radian = mMaxTurnRate;
	else if (radian < -mMaxTurnRate)
		radian = -mMaxTurnRate;
	//IncrementTankRotationAngle(RadsToDegs(radian));
	mRotationAngle += RadsToDegs(radian)*sign;

	//Usee a rotation matrix to rotate the player's heading
	C2DMatrix RotationMatrix;

	//Calculate the direction of rotation.
	RotationMatrix.Rotate(radian * sign);
	//Get the new heading.
	RotationMatrix.TransformVector2Ds(mHeading);

	//cout << "RotateHeadingByRadian -- Heading x = " << mHeading.x << " y = " << mHeading.y << endl;

	//Get the new velocity.
	RotationMatrix.TransformVector2Ds(mVelocity);

	//Side vector must always be perpendicular to the heading.
	mSide = mHeading.Perp();
}

//--------------------------------------------------------------------------------------------------
