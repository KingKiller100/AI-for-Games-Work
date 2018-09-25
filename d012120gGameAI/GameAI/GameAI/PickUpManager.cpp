#include "PickUpManager.h"
#include <SDL.h>
#include <iostream>
#include <algorithm>
#include "GameObject.h"
#include "BaseTank.h"
#include "TinyXML\tinyxml.h"
#include "Collisions.h"
#include "ObstacleManager.h"
#include <time.h>

//Initialise the instance to null.
PickUpManager* PickUpManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

PickUpManager::PickUpManager()
{
	srand((unsigned int)time(NULL));
	mTimeUntilNextPickupGeneration = 0.0f;
}

//--------------------------------------------------------------------------------------------------

PickUpManager::~PickUpManager()
{
	mInstance = NULL;

	for(unsigned int i = 0; i < mPickups.size(); i++)
		delete mPickups[i];
	mPickups.clear();
}

//--------------------------------------------------------------------------------------------------

PickUpManager* PickUpManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new PickUpManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::Init(SDL_Renderer* renderer, string mapDataPath)
{
	mRenderer = renderer;
	mInstance->LoadPickUps(mapDataPath);
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::RenderPickUps()
{
	for(unsigned int i = 0; i < mPickups.size(); i++)
	{
		mPickups[i]->Render();
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::UpdatePickUps(float deltaTime)
{
	//Increment time between pickup generation.
	mTimeUntilNextPickupGeneration += deltaTime;

	//Check if its time to generate a new pickup.
	if(mTimeUntilNextPickupGeneration > kTimeBetweenPickups)
	{
		GenerateAPickUp();
		mTimeUntilNextPickupGeneration = 0.0f;
	}

	//Remove one projectile a frame.
	if(mPickUpIndicesToDelete.size() > 0)
	{
		GameObject* pickupToDelete = mPickups.at(mPickUpIndicesToDelete[0]);
		mPickups.erase(mPickups.begin()+mPickUpIndicesToDelete[0]);
		delete pickupToDelete;
		mPickUpIndicesToDelete.erase(mPickUpIndicesToDelete.begin());
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::LoadPickUps(string mapDataPath)
{
	//Get the whole xml document.
	TiXmlDocument doc;
	if(!doc.LoadFile(mapDataPath))
	{
		cerr << doc.ErrorDesc() << endl;
	}

	//Now get the root element.
	TiXmlElement* root = doc.FirstChildElement();
	if(!root)
	{
		cerr << "Failed to load file: No root element." << endl;
		doc.Clear();
	}
	else
	{
		float x = 0;
		float y = 0;
		GAMEOBJECT_TYPE type;
		
		//Jump to the first 'objectgroup' element.
		for(TiXmlElement* groupElement = root->FirstChildElement("objectgroup"); groupElement != NULL; groupElement = groupElement->NextSiblingElement())
		{
			string name = groupElement->Attribute("name");
			if(name == "ObjectLayer")
			{
				//Jump to the first 'object' element - within 'objectgroup'
				for(TiXmlElement* objectElement = groupElement->FirstChildElement("object"); objectElement != NULL; objectElement = objectElement->NextSiblingElement())
				{
					string name = objectElement->Attribute("name");
					if(name == "PickUp")
					{
						x = (float)atof(objectElement->Attribute("x"));
						y = (float)atof(objectElement->Attribute("y"));

						//Jump to the first 'properties' element - within 'object'
						for(TiXmlElement* propertiesElement = objectElement->FirstChildElement("properties"); propertiesElement != NULL; propertiesElement = propertiesElement->NextSiblingElement())
						{
							//Loop through the 'property' elements - within 'properties'
							for(TiXmlElement* propertyElement = propertiesElement->FirstChildElement("property"); propertyElement != NULL; propertyElement = propertyElement->NextSiblingElement())
							{	
								string name = propertyElement->Attribute("name");
								if(name == "Type")
								{
									type = (GAMEOBJECT_TYPE)atoi(propertyElement->Attribute("value"));
								}
							}
						}
							
						//Add the new waypoint with the read in details.
						string path = "";
						switch(type)
						{
							case GAMEOBJECT_PICKUP_BULLETS:
								path = kBulletPickUpPath;
							break;

							case GAMEOBJECT_PICKUP_ROCKETS:
								path = kRocketPickUpPath;
							break;

							case GAMEOBJECT_PICKUP_HEALTH:
								path = kHealthPickUpPath;
							break;

							case GAMEOBJECT_PICKUP_MINES:
								path = kMinePickUpPath;
							break;

							case GAMEOBJECT_PICKUP_FUEL:
								path = kFuelPickUpPath;
							break;
						}

						GameObject* newPickup = new GameObject(mRenderer, type, Vector2D(x,y), path);
						newPickup->SetCollisionRadius(kCollisionCrateRadius);
						mPickups.push_back(newPickup);	
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::CheckForCollisions(vector<BaseTank*> listOfTanks)
{
	for(unsigned int i = 0; i < listOfTanks.size(); i++)
	{
		CheckForACollision(listOfTanks[i]);
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::CheckForACollision(BaseTank* tank)
{
	vector<Vector2D> rect = tank->GetAdjustedBoundingBox();

	for(unsigned int i = 0; i < mPickups.size(); i++)
	{
		if ((Collisions::Instance()->TriangleCollision(rect[1], rect[2], rect[3], mPickups[i]->GetPosition())) || (Collisions::Instance()->TriangleCollision(rect[0], rect[1], rect[3], mPickups[i]->GetPosition())))
		{
			if(tank->GetGameObjectType() == GAMEOBJECT_TANK)
			{
				//Prepare this pickUp for deletion.
				if(std::find(mPickUpIndicesToDelete.begin(), mPickUpIndicesToDelete.end(), i) == mPickUpIndicesToDelete.end())
					mPickUpIndicesToDelete.push_back(i);

				//Give the bonus to the colliding tank.
				switch(mPickups[i]->GetGameObjectType())
				{
					case GAMEOBJECT_PICKUP_BULLETS:
						tank->AddBullets(50);
						tank->AddToScore(SCORE_COLLECTEDPICKUP);
					break;

					case GAMEOBJECT_PICKUP_ROCKETS:
						tank->AddRockets(5);
						tank->AddToScore(SCORE_COLLECTEDPICKUP);
					break;

					case GAMEOBJECT_PICKUP_HEALTH:
						tank->AddHealth(100);
						tank->AddToScore(SCORE_COLLECTEDPICKUP);
					break;

					case GAMEOBJECT_PICKUP_MINES:
						tank->AddMines(3);
						tank->AddToScore(SCORE_COLLECTEDPICKUP);
					break;

					case GAMEOBJECT_PICKUP_FUEL:
						tank->AddFuel(30.0f);
						tank->AddToScore(SCORE_COLLECTEDPICKUP);
					break;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::GenerateAPickUp()
{
	//We only want 1 pickup ata time.
	if(mPickups.size() == 0)
	{
		string path = "";
		GAMEOBJECT_TYPE type;

		//Determine a random pickup.
		int typeToGenerate = rand()%5;

		//Get the valid path for the image.
		switch(typeToGenerate)
		{
			case 0:
				path = kBulletPickUpPath;
				type = GAMEOBJECT_PICKUP_BULLETS;
			break;

			case 1:
				path = kRocketPickUpPath;
				type = GAMEOBJECT_PICKUP_ROCKETS;
			break;

			case 2:
				path = kHealthPickUpPath;
				type = GAMEOBJECT_PICKUP_HEALTH;
			break;

			case 3:
				path = kMinePickUpPath;
				type = GAMEOBJECT_PICKUP_MINES;
			break;

			case 4:
				path = kFuelPickUpPath;
				type = GAMEOBJECT_PICKUP_FUEL;
			break;
		}

		//Get a random position to spawn the pickup.
		Vector2D spawnPos = Vector2D(rand()%kScreenWidth, rand()%kScreenHeight);
		
		//Ensure this position is valid.
		while(CollisionWithAnObstacle(spawnPos))
		{
			spawnPos = Vector2D(rand()%kScreenWidth, rand()%kScreenHeight);
		}

		//Add the pickup to our list of pickups.
		GameObject* newPickup = new GameObject(mRenderer, type, spawnPos, path);
		newPickup->SetCollisionRadius(kCollisionCrateRadius);
		mPickups.push_back(newPickup);	
	}
}

//--------------------------------------------------------------------------------------------------

bool PickUpManager::CollisionWithAnObstacle(Vector2D positionToCheck)
{
	//Calculate positions around the spawn position to ensure pickups do not spawn in obstacles.
	Vector2D modifiedPositionLeft	= positionToCheck;
	modifiedPositionLeft.x -= kSafeCrateDistance;
	Vector2D modifiedPositionRight	= positionToCheck;
	modifiedPositionRight.x += kSafeCrateDistance;
	Vector2D modifiedPositionTop	= positionToCheck;
	modifiedPositionTop.y -= kSafeCrateDistance;
	Vector2D modifiedPositionBottom = positionToCheck;
	modifiedPositionBottom.y += kSafeCrateDistance;

	//Loop through the obstacles in the scene checking is this position is within an obstacle.
	for( unsigned int i = 0; i < ObstacleManager::Instance()->GetObstacles().size(); i++ )
	{
		GameObject* currentObstacle = ObstacleManager::Instance()->GetObstacles().at(i);
		vector<Vector2D> rect = currentObstacle->GetAdjustedBoundingBox();

		if ((Collisions::Instance()->TriangleCollision(rect[1], rect[2], rect[3], positionToCheck)) || (Collisions::Instance()->TriangleCollision(rect[0], rect[1], rect[3], positionToCheck)))
		{
			//cout << "Tried pickup placement and FAILED" << endl;
			return true;
		}
	}

	//If we reach this point there was no collision.
	//cout << "Tried pickup placement and PASSED" << endl;
	return false;
}

//--------------------------------------------------------------------------------------------------
