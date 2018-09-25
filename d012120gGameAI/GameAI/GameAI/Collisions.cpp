#include "Collisions.h"
#include "GameObject.h"
#include "Commons.h"
#include <iostream>

//Initialise the instance to null.
Collisions* Collisions::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

Collisions::Collisions()
{
}

//--------------------------------------------------------------------------------------------------

Collisions::~Collisions()
{
	mInstance = NULL;
}

//--------------------------------------------------------------------------------------------------

Collisions* Collisions::Instance()
{
	if(!mInstance)
	{
		mInstance = new Collisions;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

bool Collisions::Circle(GameObject* object1, GameObject* object2)
{
	Vector2D vec = Vector2D( (object1->GetCentralPosition().x - object2->GetCentralPosition().x),
							 (object1->GetCentralPosition().y - object2->GetCentralPosition().y));
	double distanceSq = (vec.x*vec.x)+(vec.y*vec.y);

	double combinedDistanceSq = (object1->GetCollisionRadiusSq()+object2->GetCollisionRadiusSq());
	return distanceSq < combinedDistanceSq;
}		

//--------------------------------------------------------------------------------------------------

//bool Collisions::Box(Character* character1, Character* character2)
//{
/*	Rect2D rect1 = character1->GetCollisionBox();
	Rect2D rect2 = character2->GetCollisionBox();

	if(rect1.x > (rect2.x+rect2.width))
		return false;
	else if((rect1.x+rect1.width) < rect2.x)
		return false;
	else if(rect1.y > (rect2.y+rect2.height))
		return false;
	else if((rect1.y+rect1.height) < rect2.y)
		return false;

	return true;
*/

//	return false;
//}

//--------------------------------------------------------------------------------------------------

bool Collisions::PointInBox(Vector2D point, Rect2D rect2)
{
	if( point.x > rect2.x &&
		point.x < (rect2.x+rect2.width) &&
		point.y > rect2.y &&
		point.y < (rect2.y+rect2.height) )
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

bool Collisions::TriangleCollision(Vector2D A, Vector2D B, Vector2D C, Vector2D P)
{
	//http://www.blackpawn.com/texts/pointinpoly/

	//Compute Vectors
	Vector2D v0 = C - A;
	Vector2D v1 = B - A;
	Vector2D v2 = P - A;

	//Compute dot product
	float dot00 = v0.Dot(v0);
	float dot01 = v0.Dot(v1);
	float dot02 = v0.Dot(v2);
	float dot11 = v1.Dot(v1);
	float dot12 = v1.Dot(v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}

//--------------------------------------------------------------------------------------------------
