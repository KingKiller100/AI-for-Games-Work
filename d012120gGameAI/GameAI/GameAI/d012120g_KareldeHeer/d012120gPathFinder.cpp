#include "d012120gPathFinder.h"

d012120gPathFinder::d012120gPathFinder()
{
	SetEdgecost();
}

d012120gPathFinder::~d012120gPathFinder()
{
	for (auto listItem : edgeCost_List)
		delete listItem;
	edgeCost_List.clear();

	for (auto listItem : open_List)
		delete listItem;
	open_List.clear();

	for (auto listItem : closed_List)
		delete listItem;
	closed_List.clear();
}

vector<Vector2D> d012120gPathFinder::GetPathBetween(Vector2D startPosition, Vector2D endPosition)
{
	open_List.clear();
	closed_List.clear();

	vector<Vector2D> path;
	Waypoint* nearestToTank = GetNearestWayPointToPosition(startPosition);
	Waypoint* nearestToEnd = GetNearestWayPointToPosition(endPosition);

	if (nearestToTank == nullptr || nearestToTank == nearestToEnd)
	{
		path.push_back(endPosition);
		return path;
	}

	// Add nerest waypoint to the OPEN list
	open_List.push_back(new d012120gAStarNode(nearestToTank, nullptr, 0.0f));

	d012120gAStarNode* currentNode = nullptr;

	while (!open_List.empty())
	{
		for (auto listItem : open_List)
		{
			//If we dont have a current node, or the cost if tgis one is less than the stored one
			//store this new node
			if (currentNode == nullptr || listItem->cost < currentNode->cost)
			{
				currentNode = listItem;
			}
		}

		if (currentNode->internalWayPoint->GetID() == nearestToEnd->GetID())
		{
			path = ConstructPath(currentNode, endPosition);
			return path;
		}

		vector<int> waypointIDs = currentNode->internalWayPoint->GetConnectedWaypointIDs();

		for (auto id : waypointIDs)
		{
			Waypoint* usedWaypoint = WaypointManager::Instance()->GetWaypointWithID(id);
			if (!IsInList(open_List, usedWaypoint) && !IsInList(closed_List, usedWaypoint))
			{
				double g = GetCostBetweenWayPoints(currentNode->internalWayPoint, edgeCost_List[id]->wayPointEnd);

				double h = GetHeuristCost(currentNode->internalWayPoint->GetPosition(), endPosition);

				double f = g + h;

				open_List.push_back(new d012120gAStarNode(usedWaypoint, currentNode, f));
			}
		}

		closed_List.push_back(currentNode);

		vector<d012120gAStarNode*>::iterator iter = open_List.begin();
		while (iter != open_List.end())
		{
			if (*iter == currentNode)
			{
				iter = open_List.erase(iter);
				break;
			}
			else
				++iter;
		}
		currentNode = nullptr;
	}
	return path;
}

void d012120gPathFinder::SetEdgecost()
{
	vector<Waypoint*> allWayPoints = WaypointManager::Instance()->GetAllWaypoints();
	for (auto wayPoints : allWayPoints)
	{
		vector<int> connectedIDs = wayPoints->GetConnectedWaypointIDs();
		for (auto id : connectedIDs)
		{
				Waypoint* currentEndWaypoint = WaypointManager::Instance()->GetWaypointWithID(id);
				edgeCost_List.push_back(new d012120gEdgeCost(wayPoints, currentEndWaypoint, wayPoints->GetPosition().Distance(currentEndWaypoint->GetPosition())));
		}
	}
}

Waypoint * d012120gPathFinder::GetNearestWayPointToPosition(Vector2D position)
{
	vector<Waypoint*> wayPoints = WaypointManager::Instance()->GetAllWaypoints();
	float nearestDistance = MaxFloat;
	Waypoint* nearestWayPoint = nullptr;

	for (auto wayP : wayPoints)
	{
		Vector2D vecBetweenPoints = position - wayP->GetPosition();
		if (vecBetweenPoints.Length() < nearestDistance)
		{
			nearestDistance = vecBetweenPoints.Length();
			nearestWayPoint = wayP;
		}
	}

	return nearestWayPoint;
}

double d012120gPathFinder::GetCostBetweenWayPoints(Waypoint * from, Waypoint * to)
{
	for (auto edgeCost : edgeCost_List)
	{
		if (edgeCost->wayPointStart == from && edgeCost->wayPointEnd == to)
		{
			return edgeCost->cost;
		}
	}
	return MaxFloat;
}

vector<Vector2D> d012120gPathFinder::ConstructPath(d012120gAStarNode * targetNode, Vector2D endPos)
{
	vector<Vector2D> path;
	vector<Vector2D> pathInReverse;

	pathInReverse.push_back(endPos);
	d012120gAStarNode* currentNode = targetNode;

	while (currentNode != nullptr)
	{
		// Add the position of this waypoint
		pathInReverse.push_back(currentNode->internalWayPoint->GetPosition());
		currentNode = currentNode->parentWayPoint;
	}
	// Take the path in reverse and reverse the elementsto give us the path in order
	vector<Vector2D>::iterator iter = pathInReverse.end();
	while (iter != pathInReverse.begin())
		path.push_back(*(--iter));

	return path;
}

bool d012120gPathFinder::IsInList(vector<d012120gAStarNode*> listToCheck, Waypoint * wayPointToCheck)
{
	for (auto itemToCheck : listToCheck)
	{
		if (itemToCheck->internalWayPoint->GetID() == wayPointToCheck->GetID())
		{
			return true;
		}
	}
	return false;
}

double d012120gPathFinder::GetHeuristCost(Vector2D pos1, Vector2D pos2)
{
	Vector2D vecBetweenPoints = pos1 - pos2;

	return vecBetweenPoints.Length();
}

