#include "GameScreenLevel1.h"
#include <iostream>
#include "LevelMap.h"
#include "Texture2D.h"
#include "Collisions.h"
#include "WaypointManager.h"
#include "ObstacleManager.h"
#include "TankManager.h"
#include "ProjectileManager.h"
#include "PickUpManager.h"
#include "TinyXML\tinyxml.h"

using namespace::std;

string GameScreenLevel1::ScorePath = "Scores/TestScores.txt";

//--------------------------------------------------------------------------------------------------

GameScreenLevel1::GameScreenLevel1(SDL_Renderer* renderer) : GameScreen(renderer)
{
	srand(NULL);

	//Load in file paths from LevelData.xml
	LoadLevelData();

	//Set the level map.
	mLevelMap = new LevelMap(renderer, mMapPath);

	//Set up the waypoints.
	WaypointManager::Instance()->Init(renderer, mMapPath);

	//Set up the obstacles.
	ObstacleManager::Instance()->Init(renderer, mMapPath);

	//Set up the tanks.
	TankManager::Instance()->Init(renderer, mTanksPath);

	//Set up PickUps
	PickUpManager::Instance()->Init(renderer, mMapPath);

	//Set up projectiles that start on screen.
	ProjectileManager::Instance()->Init(renderer);
}

//--------------------------------------------------------------------------------------------------

GameScreenLevel1::~GameScreenLevel1()
{
	//Level map.
	delete mLevelMap;
	mLevelMap = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreenLevel1::Render()
{
	//Draw the background.
	mLevelMap->Render();

	//Draw the bullets.
	ProjectileManager::Instance()->RenderProjectiles();
	
	//Draw the obstacles over the level map.
	ObstacleManager::Instance()->RenderObstacles();

	//Draw the waypoints.
	WaypointManager::Instance()->RenderWaypoints();

	//Draw the pickUps.
	PickUpManager::Instance()->RenderPickUps();

	//Draw the tanks after everything else.
	TankManager::Instance()->RenderTanks();
}

//--------------------------------------------------------------------------------------------------

void GameScreenLevel1::Update(float deltaTime, SDL_Event e)
{
	switch(e.type)
	{
		//Deal with keyboard input.
		case SDL_KEYUP:
			switch(e.key.keysym.sym)
			{
				case SDLK_DOWN:
				break;

				default:
				break;
			}
		break;
	}

	//Update the obstacles.
	ObstacleManager::Instance()->UpdateObstacles(deltaTime, e);

	//Update the tanks.
	TankManager::Instance()->UpdateTanks(deltaTime, e);

	//Update the bullets.
	ProjectileManager::Instance()->UpdateProjectiles(deltaTime);

	//Do collision checks.
	ProjectileManager::Instance()->CheckForCollisions(ObstacleManager::Instance()->GetObstacles());
	ProjectileManager::Instance()->CheckForCollisions(TankManager::Instance()->GetTanks());

	//Update the pickups.
	PickUpManager::Instance()->UpdatePickUps(deltaTime);
	PickUpManager::Instance()->CheckForCollisions(TankManager::Instance()->GetTanks());

}

//--------------------------------------------------------------------------------------------------

void GameScreenLevel1::LoadLevelData()
{
	//Get the whole xml document.
	TiXmlDocument doc;
	if(!doc.LoadFile("XML Data Files/LevelData.xml"))
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
		//Jump to the the 'map' element - within 'data'
		TiXmlElement* mapElement = root->FirstChildElement("map");
		mMapPath = mapElement->Attribute("mapPath");

		//Jump to the the 'tanks' element - within 'data'
		TiXmlElement* tanksElement = root->FirstChildElement("tanks");
		mTanksPath = tanksElement->Attribute("tanksPath");

		//Jump to the the 'score' element - within 'data'
		TiXmlElement* scoreElement = root->FirstChildElement("score");
		ScorePath = scoreElement->Attribute("scorePath");
	}
}

//--------------------------------------------------------------------------------------------------