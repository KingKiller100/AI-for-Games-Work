#pragma once

#include "../Waypoint.h"
#include "../WaypointManager.h"

// Internal structure to house the cost of moving along an edge
struct d012120gEdgeCost
{
	Waypoint* wayPointStart;
	Waypoint* wayPointEnd;
	double cost;

	d012120gEdgeCost(Waypoint* startPoint, Waypoint* endPoint, double newCost)
	{
		wayPointStart = startPoint;
		wayPointEnd = endPoint;
		cost = newCost;
	}
};

struct d012120gAStarNode
{
	Waypoint* internalWayPoint;
	d012120gAStarNode* parentWayPoint;
	double cost;

	d012120gAStarNode(Waypoint* _currentWayPoint, d012120gAStarNode* _parentWayPoint, double _newCost)
	{
		internalWayPoint = _currentWayPoint;
		parentWayPoint = _parentWayPoint;
		cost = _newCost;
	}
};

enum d012120gWeaponType
{
	WEAPONTYPE_ROCKETS = 0,
	WEAPONTYPE_BULLETS = 1,
	WEAPONTYPE_MINES = 2
};

enum d012120gSurvivalStates
{
	TANK_ATTACKSTATE = 0,
	TANK_PATROLSTATE = 1,
	TANK_FLEESTATE = 2,
	Tank_EVADESTATE = 3
};
