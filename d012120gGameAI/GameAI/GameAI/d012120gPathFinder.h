#pragma once
#include "d012120gStructures.h"
#include <vector>

class PathFinder
{
public:
	PathFinder();
	~PathFinder();

private:
	std::vector<EdgeCost*> edgeCost_List;
	std::vector<AStarNode*> edgeCost_List;
	std::vector<AStarNode*> edgeCost_List;

};

PathFinder::PathFinder()
{
}

PathFinder::~PathFinder()
{
}