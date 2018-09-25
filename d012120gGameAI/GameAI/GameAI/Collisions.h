#ifndef _COLLISIONS_H
#define _COLLISIONS_H

#include "Commons.h"

//--------------------------------------------------------------------------------------------------

class GameObject;

//--------------------------------------------------------------------------------------------------
class Collisions
{
	
//--------------------------------------------------------------------------------------------------
public:
	~Collisions();

	static Collisions* Instance();

	bool Circle(GameObject* object1, GameObject* object2);
	//bool Box(Character* character1, Character* character2);
	bool PointInBox(Vector2D point, Rect2D rect2);
	bool TriangleCollision(Vector2D A, Vector2D B, Vector2D C, Vector2D P);

//--------------------------------------------------------------------------------------------------
private:
	Collisions();

//--------------------------------------------------------------------------------------------------
private:
	static Collisions* mInstance;

};

//--------------------------------------------------------------------------------------------------
#endif //_COLLISIONS_H