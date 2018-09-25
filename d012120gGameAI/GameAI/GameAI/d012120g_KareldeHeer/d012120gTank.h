#ifndef d012120gTANK_H
#define d012120gTANK_H

#include "../BaseTank.h"
#include <SDL.h>
#include "../Commons.h"
#include "../ObstacleManager.h"
#include "../PickUpManager.h"
#include "d012120gSteeringBehaviours.h"
#include "d012120gPathFinder.h"
#include "d012120gDecisionManager.h"


//---------------------------------------------------------------

class d012120gTank : public BaseTank
{
	//---------------------------------------------------------------
public:
	d012120gTank(SDL_Renderer* renderer, TankSetupDetails details);
	~d012120gTank();

	void ChangeState(BASE_TANK_STATE newState);

	void Update(float deltaTime, SDL_Event e);
	void Render();

	void RotateHeadingByRadian(double radian, int sign);

	//---------------------------------------------------------------
protected:
	void	MoveInHeadingDirection(float deltaTime);
	void RotateToFacePosition(float deltaTime);
private:

	//Vector2D mouseTargetPos;
	Vector2D force;
	double angle;
	double mCurrentSpeed;
};

//---------------------------------------------------------------
#endif //d012120gTANK_H