#pragma once

#include "Waypoint.h"
#include "WaypointManager.h"

// Internal structure to house the cost of moving along an edge
struct EdgeCost
{
	Waypoint* wayPointStart;
	Waypoint* wayPointEnd;
	double cost;

	EdgeCost(Waypoint* startPoint, Waypoint* endPoint, double newCost)
	{
		wayPointStart = startPoint;
		wayPointEnd = endPoint;
		cost = newCost;
	}
};

struct AStarNode
{
	Waypoint* internalWayPoint;
	AStarNode* parentWayPoint;
	double cost;

	AStarNode(Waypoint* _currentWayPoint, AStarNode* _parentWayPoint, double _newCost)
	{
		internalWayPoint = _currentWayPoint;
		parentWayPoint = _parentWayPoint;
		cost = _newCost;
	}
};