#pragma once
#include "d012120gStructures.h"
#include "../Commons.h"
#include <vector>

class d012120gPathFinder
{
public:
	d012120gPathFinder();
	~d012120gPathFinder();

	vector<Vector2D>	GetPathBetween(Vector2D startPosition, Vector2D endPosition);

private:
	void				SetEdgecost();
	Waypoint*			GetNearestWayPointToPosition(Vector2D position);
	double				GetCostBetweenWayPoints(Waypoint* from, Waypoint* to);
	vector<Vector2D>	ConstructPath(d012120gAStarNode* targetNode, Vector2D endPos);

	bool				IsInList(vector<d012120gAStarNode*> listToCheck, Waypoint* wayPointToCheck);
	double				GetHeuristCost(Vector2D pos1, Vector2D pos2);

private:
	vector<d012120gEdgeCost*>	edgeCost_List;
	vector<d012120gAStarNode*>	open_List;
	vector<d012120gAStarNode*>	closed_List;
};
