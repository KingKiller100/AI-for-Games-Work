#pragma once
#include "GameObject.h"
#include "BaseTank.h"
#include "PickUpManager.h"
class d012120gDecisionMaking
{
public:
	d012120gDecisionMaking();
	~d012120gDecisionMaking();
	void Update();
private:
	vector<GameObject*> onScreenPickUps;
};

