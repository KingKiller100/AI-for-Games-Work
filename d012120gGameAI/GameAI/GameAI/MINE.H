#ifndef MINE_H
#define MINE_H

#include <SDL.h>
#include "Projectile.h"

class BaseTank;
class Texture2D;

//--------------------------------------------------------------------------------------------------

class Mine : public Projectile
{
//--------------------------------------------------------------------------------------------------
public:
	Mine(SDL_Renderer* renderer, ProjectileSetupDetails details, BaseTank* firer);
	~Mine();

	void		Update(float deltaTime);
	void		Render();

	void		Explode();
	bool		HasExploded();		
//--------------------------------------------------------------------------------------------------
private:
	SDL_Rect GetCurrentExplosionSprite();

//--------------------------------------------------------------------------------------------------
private:
	Texture2D*			mExplosionSpritesheet;				//The explosion.
	int					mExplosionSingleSpriteHeight;
	int					mExplosionSingleSpriteWidth;
	int					mExplosionFrame;
	float				mExplosionTime;
	Vector2D			mExplosionOffset;
	float				mExplosionDelay;
};

//--------------------------------------------------------------------------------------------------
#endif //Projectile_H