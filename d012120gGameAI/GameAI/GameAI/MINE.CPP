#include "Mine.h"
#include "Projectile.h"
#include "GameObject.h"
#include "BaseTank.h"
#include "Commons.h"
#include <iostream>

//--------------------------------------------------------------------------------------------------

Mine::Mine(SDL_Renderer* renderer, ProjectileSetupDetails details, BaseTank* firer)
	: Projectile(renderer, details, firer)
{
	mExplosionSpritesheet			= new Texture2D(renderer);
	mExplosionSpritesheet->LoadFromFile(kExplosionImagePath);
	mExplosionSingleSpriteWidth		= mExplosionSpritesheet->GetWidth();
	mExplosionSingleSpriteHeight	= mExplosionSpritesheet->GetHeight()/kNumberOfSpritesPerExplosion;
	mExplosionTime					= 0.0f;
	mExplosionOffset.x				= -(mExplosionSingleSpriteWidth*0.5f);
	mExplosionOffset.y				= -(mExplosionSingleSpriteHeight*0.5f);
	mExplosionFrame					= 0;

	UpdateAdjustedBoundingBox();

}

//--------------------------------------------------------------------------------------------------

Mine::~Mine()
{
	delete mExplosionSpritesheet;
	mExplosionSpritesheet = NULL;
}

//--------------------------------------------------------------------------------------------------

void Mine::Update(float deltaTime)
{
	if(!mAlive)
	{
		//Increment the time the current sprite has been displayed.
		mExplosionTime += deltaTime;
			
		//If the requisite time has passed, increment the frame number.
		if(mExplosionTime > kExplosionDelay)
		{
  			mExplosionFrame++;
			mExplosionTime = 0.0f;
		}
	}
	else
	{
		Projectile::Update(deltaTime);
	}
}

//--------------------------------------------------------------------------------------------------

void Mine::Render()
{
	if(!mAlive)
	{
		//Draw the explosion image.
		SDL_Rect destRect = {(int)(GetCentralPosition().x+mExplosionOffset.x), (int)(GetCentralPosition().y+mExplosionOffset.y), mExplosionSingleSpriteWidth, mExplosionSingleSpriteHeight};
		mExplosionSpritesheet->Render(GetCurrentExplosionSprite(), destRect, 0); 

		//cout << "RENDER MINE EXPOSION" << endl;
	}
	else
	{
		Projectile::Render();
	}
}

//--------------------------------------------------------------------------------------------------

SDL_Rect Mine::GetCurrentExplosionSprite()
{
	//NOTE: Spritesheets are currently made of a single column of images.
	SDL_Rect portionOfSpritesheet = {0, 0, mExplosionSingleSpriteWidth, mExplosionSingleSpriteHeight};

	portionOfSpritesheet.y = mExplosionFrame * mExplosionSingleSpriteHeight;

	//DEBUG: See which frame is being output.
	//cout << portionOfSpritesheet.y << endl;
	return portionOfSpritesheet;
}

//--------------------------------------------------------------------------------------------------

void Mine::Explode()
{
	mAlive = false; 
	mExploding = true; 
	mExplosionFrame = 0;
}

//--------------------------------------------------------------------------------------------------

bool Mine::HasExploded()
{
	return (mExploding && mExplosionFrame >= kNumberOfSpritesPerExplosion);
}

//--------------------------------------------------------------------------------------------------