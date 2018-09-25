#include "d012120gDecisionMaking.h"



d012120gDecisionMaking::d012120gDecisionMaking()
{
	
}


d012120gDecisionMaking::~d012120gDecisionMaking()
{
}

void d012120gDecisionMaking::Update()
{
	onScreenPickUps = PickUpManager::Instance()->GetAllPickUps;

	if (onScreenPickUps.size() == 0)
	{
		
	}
}
