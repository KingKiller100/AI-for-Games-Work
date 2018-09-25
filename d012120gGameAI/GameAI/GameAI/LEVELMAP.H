#ifndef _LEVELMAP_H
#define _LEVELMAP_H

#include <SDL.h>
#include "Constants.h"
#include <string>

using namespace::std;

//--------------------------------------------------------------------------------------------------

class Texture2D;

//--------------------------------------------------------------------------------------------------

class LevelMap
{

	//---------------------------------------------------------------
public:
	LevelMap(SDL_Renderer* renderer, string mapDataPath);
	~LevelMap();

	void Render();

	//---------------------------------------------------------------
private:
	SDL_Rect GetTileSourceRect(int id);

	//---------------------------------------------------------------
private:
	Texture2D*	mSpritesheet;
	int**		mMap;
	int			mWidth;
	int			mHeight;
	int			mTileWidth;
	int			mTileHeight;
};

//--------------------------------------------------------------------------------------------------
#endif //_LEVEHMAP_H