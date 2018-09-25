#include "d012120gSteeringBehaviours.h"
#include "ObstacleManager.h"
#include "Collisions.h"
#include "C2DMatrix.h"

SteeringBehaviours::SteeringBehaviours(BaseTank * mTank)
{
	this->mTank = mTank;

	feelerDisplacement = 25.0;
	sideFeelerDisplacement = 22.5;

	mouseTargetPos = Vector2D(0,0);

	feelers.resize(8);

}

SteeringBehaviours::~SteeringBehaviours()
{
}


Vector2D SteeringBehaviours::Seek(Vector2D targetPosition)
{
	Vector2D desiredVelocity = Vec2DNormalize(targetPosition - mTank->GetCentralPosition()) * mTank->GetMaxSpeed();

	return desiredVelocity - mTank->GetVelocity();
}

Vector2D SteeringBehaviours::Flee(Vector2D targetPosition)
{
	Vector2D desiredVelocity = Vec2DNormalize(mTank->GetCentralPosition() - targetPosition) * mTank->GetMaxSpeed();
	return (desiredVelocity - mTank->GetVelocity());
}

Vector2D SteeringBehaviours::ProximityFlee(Vector2D targetPosition)
{
	// Only flees when a target is within a specific proximity to the tank
	const double panicDistance = 100.0 * 100.0;

	if (Vec2DDistanceSq(mTank->GetCentralPosition(), targetPosition) > panicDistance)
		return Vector2D(0, 0);

	Vector2D desiredVelocity = Vec2DNormalize(mTank->GetCentralPosition() - targetPosition) * mTank->GetMaxSpeed();
	return (desiredVelocity - mTank->GetVelocity());
}

Vector2D SteeringBehaviours::Arrive(Vector2D targetPosition, Deceleration decelSpeed)
{
	Vector2D toTarget = targetPosition - mTank->GetCentralPosition();

	// Calculates distance between tank and target position
	double distance = toTarget.Length();

	if (distance > 0)
	{
		// Performs fine tweaking required to Deceleration enum due to it being enumerated as a int
		const double decelerationTweaker = 2.3;

		// Calculates the speed needed to reach the target using decleration speed
		double speed = distance / ((double)decelSpeed * decelerationTweaker);

		// Limits velocity so it doesn't exceed max speed
		speed = min(speed, mTank->GetMaxSpeed());

		Vector2D desiredVelocity = toTarget * speed / distance;

		return (desiredVelocity - mTank->GetVelocity());
	}

	return Vector2D(0.0, 0.0);
}

Vector2D SteeringBehaviours::Pursuit(BaseTank * prey)
{
	// if evader is directly ahead of this tank, then we just seek the prey's current position
	Vector2D toPrey = prey->GetCentralPosition() - mTank->GetCentralPosition();

	double relativeHeading = mTank->GetHeading().Dot(prey->GetHeading());

	if ((toPrey.Dot (mTank->GetHeading()) > 0) && (relativeHeading < -0.95)) // acos(0.95) = 18 degrees
	{
		return Seek(prey->GetCentralPosition());
	}

	// Only seeks the current so we'll predict wheere the prey tank will be

	// lookahead time is propertional to the distance between the predator tank and prey tank
	// which is inversely propertional to the sum of the aent's velocities
	double lookAheadTime = toPrey.Length() / (mTank->GetMaxSpeed() + prey->GetCurrentSpeed());
	lookAheadTime += TurnAroundTime(mTank, prey->GetCentralPosition());

	// now seek to the predicted future position of the prey
	return Seek(prey->GetCentralPosition() + prey->GetVelocity() * lookAheadTime);
}

Vector2D SteeringBehaviours::Evade(BaseTank * predator)
{
	Vector2D toPredator = predator->GetCentralPosition() - mTank->GetCentralPosition();

	// Look ahead time is proportional to the distance between the preedator
	// and the prey; and is inversely proportional to the sum of the agent's
	// velocities
	double lookAheadTime = toPredator.Length() / (mTank->GetMaxSpeed() + predator->GetCurrentSpeed());
	lookAheadTime += TurnAroundTime(mTank, predator->GetCentralPosition());

	// Now flee away from predicted future position of the predator
	return Flee(predator->GetCentralPosition() + predator->GetVelocity() * lookAheadTime);
}


Vector2D SteeringBehaviours::ObstacleAvoidance()
{
	CreateFeelers();
	Vector2D resultingForce = Vector2D(0, 0);
	vector<GameObject*> listOfObjects = ObstacleManager::Instance()->GetObstacles();
	for (auto feelers : feelers)
	{
		for (auto buildings : ObstacleManager::Instance()->GetObstacles())
		{
			vector<Vector2D> buidlingRect = buildings->GetAdjustedBoundingBox();
			if (Collisions::Instance()->TriangleCollision(buidlingRect[1], buidlingRect[2], buidlingRect[3], feelers) ||
				Collisions::Instance()->TriangleCollision(buidlingRect[0], buidlingRect[1], buidlingRect[3], feelers))
			{
				resultingForce -= feelers - mTank->GetCentralPosition();
			}
		}
	}
	return resultingForce * 7.0;
}


double SteeringBehaviours::TurnAroundTime(BaseTank * pAgent, Vector2D targetPosition)
{
	// Determine the normalized vector to the target
	Vector2D toTarget = Vec2DNormalize(targetPosition - pAgent->GetCentralPosition());

	double dot = pAgent->GetHeading().Dot(toTarget);

	const double coefficient = 0.5;

	return (dot - 1.0) * -coefficient;
}

void SteeringBehaviours::CreateFeelers()
{
	feelers[0] = mTank->GetCentralPosition() + feelerDisplacement * mTank->GetHeading();
	feelers[1] = mTank->GetCentralPosition() + feelerDisplacement * RotateVectorAngle (mTank->GetHeading(), 2);
	feelers[2] = mTank->GetCentralPosition() + feelerDisplacement * RotateVectorAngle (mTank->GetHeading(), 0.4);
	feelers[3] = mTank->GetCentralPosition() + feelerDisplacement * RotateVectorAngle (mTank->GetHeading(), -0.4);
	feelers[4] = mTank->GetCentralPosition() + feelerDisplacement * RotateVectorAngle (mTank->GetHeading(), 1.6);
	feelers[5] = mTank->GetCentralPosition() + feelerDisplacement * RotateVectorAngle (mTank->GetHeading(), -1.6);
	feelers[6] = mTank->GetCentralPosition() + sideFeelerDisplacement * RotateVectorAngle (mTank->GetHeading(), 1);
	feelers[7] = mTank->GetCentralPosition() + sideFeelerDisplacement * RotateVectorAngle (mTank->GetHeading(), 3);
}

Vector2D SteeringBehaviours::RotateVectorAngle(Vector2D vect, double theta)
{
	C2DMatrix feelerMatrix;
	feelerMatrix.Rotate(HalfPi * theta);
	feelerMatrix.TransformVector2Ds(vect);
	return vect;
}