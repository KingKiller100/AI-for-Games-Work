#ifndef STEERINGBEHAVIOURS_H
#define STEERINGBEHAVIOURS_H

#include "../Commons.h"
#include <SDL.h>
#include <vector>
#include "../BaseTank.h"

class d012120gSteeringBehaviours
{
public:
	d012120gSteeringBehaviours(BaseTank * mTank);
	~d012120gSteeringBehaviours();

	// Behaviours
	Vector2D Seek(Vector2D targetPosition);
	Vector2D Flee(Vector2D targetPosition);
	Vector2D ProximityFlee(Vector2D targetPosition);
	Vector2D Arrive(Vector2D targetPosition, Deceleration decelSpeed);
	Vector2D Pursuit(BaseTank * prey);
	Vector2D Evade(BaseTank * predator);
	Vector2D ObstacleAvoidance();
	
	std::vector<Vector2D> GetFeelers() { return feelers; }

private:
	BaseTank* mTank;
	Vector2D mouseTargetPos;
	Vector2D force;
	std::vector<Vector2D> feelers;
	double horizontalFeelerDisplacement;
	double verticalFeelerDisplacement;

	//vector<BaseTank *> mVisibleEnemyTanks;

private:
	// Allows extra time for the tank to turn around before moving towards a target
	double TurnAroundTime(BaseTank* pAgent, Vector2D targetPosition);

	// Feeler code
	void CreateFeelers();
	Vector2D RotateVectorAngle(Vector2D vect, double radians);
};


#endif // !STEERINGBEHAVIOURS_H