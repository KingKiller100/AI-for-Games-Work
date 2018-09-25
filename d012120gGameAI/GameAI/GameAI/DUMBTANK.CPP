#include "DumbTank.h"
#include "BaseTank.h"
#include "TankManager.h"
#include "C2DMatrix.h"

//--------------------------------------------------------------------------------------------------

DumbTank::DumbTank(SDL_Renderer* renderer, TankSetupDetails details) : BaseTank(renderer, details)
{
}

//--------------------------------------------------------------------------------------------------

DumbTank::~DumbTank()
{
}

//--------------------------------------------------------------------------------------------------

void DumbTank::ChangeState(BASE_TANK_STATE newState)
{
	BaseTank::ChangeState(newState);
}

//--------------------------------------------------------------------------------------------------

void DumbTank::Update(float deltaTime, SDL_Event e)
{
	//This is a dumb tank. Do NOT copy this approach.

	//Did we see a tank?
	if(mTanksICanSee.size() == 0)
	{
		ChangeState(TANKSTATE_IDLE);

		//If there are no visible tanks, then keep moving.

		//Check if we reached position before turning.
		if(mPosition.y < kTileDimensions)
		{
			if(mHeading.y < 0.9999f)
				RotateHeadingByRadian(3.0f, -1, deltaTime);
			else
				mHeading.y = 1.0f;

			mVelocity = mHeading;
		}
		else if(mPosition.y > kScreenHeight-(kTileDimensions*2))
		{
			if(mHeading.y > -0.99999f)
				RotateHeadingByRadian(3.0f, -1, deltaTime);
			else
				mHeading.y = -1.0f;

			mVelocity = mHeading;
		}

		
			mCurrentSpeed = GetMaxSpeed();
	}
	else
	{
		//Rotate man to face enemy tank.
		Vector2D toTarget = mTanksICanSee[0]->GetCentralPosition()-GetCentralPosition();
		toTarget.Normalize();
		double dot = toTarget.Dot(mManFireDirection);
		if(dot < 0.95f)
			RotateManByRadian(kManTurnRate, -1, deltaTime);

		//Otherwise stop moving and fire at the visible tank.
		mVelocity = Vector2D();
		ChangeState(TANKSTATE_MANFIRE);
	}

	BaseTank::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void DumbTank::MoveInHeadingDirection(float deltaTime)
{
	//Get the force that propels in current heading.
	Vector2D force = (mHeading*mCurrentSpeed)-mVelocity;

	//Acceleration = Force/Mass
	Vector2D acceleration = force/GetMass();

	//Update velocity.
	mVelocity += acceleration * deltaTime;
	
	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed()); //TODO: Add Penalty for going faster than MAX Speed.

	//Finally, update the position.
	Vector2D newPosition = GetPosition();
		newPosition.x += mVelocity.x*deltaTime;
		newPosition.y += (mVelocity.y/**-1.0f*/)*deltaTime;	//Y flipped as adding to Y moves down screen.
	SetPosition(newPosition);
}

//--------------------------------------------------------------------------------------------------