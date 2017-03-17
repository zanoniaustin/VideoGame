#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <math.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include "MediaManager.hpp"

using namespace std;

//Terrain constants
const int MAPSIZE= 150;//50*64=3200X3200PX actual map size
const int TILE_HEIGHT =16;
const int TILE_WIDTH = 16;

//types of tiles that exist
//no longer true
enum TileType {
	Floor_TL
	
	Floor_TM
	Floor_TM1
	FLoor_TM2
	
	Floor_TR
	
	Isle_TL
	Isle_TM
	Isle_TR
	
	Stair_Floor
	Stair_Shadow
	Stair
	Stair1
	
	Pool_TL
	Pool_TM
	Pool TR
	
	Pool_ITL
	Pool_ITR
	Pool_IBL
	Pool_IBR
	
	Pool_Isle_Vert_top
	Pool_Isle_TL
	Pool_Isle_TM
	Pool_Isle_TR
	
	//ROW
	
	Floor_L
	Floor_M
	Floor_M1
	Floor_M2
	Floor_R
	Isle_L
	//Blank
	Isle_R
	Floor_Shadow
	Floor_ITL
	Floor_ITR
	
	Stair
	
	};

bool isCollided(SDL_Rect r1, SDL_Rect r2){
	//set bounds
	int left1 = r1.x;
	int left2 = r2.x;
	int right1 = r1.x+r1.w;
	int right2 = r2.x+r2.w;
	int top1 = r1.y;
	int top2 = r2.y;
	int bot1 = r1.y+r1.h;
	int bot2 = r2.y+r2.h;
	
	//test if r1 is outside r2 (return false if it is on any axis)
	if(bot1<=top2)return false;
	if(top1>=bot2)return false;
	if(right1<=left2)return false; 
	if(left1>=right2)return false;
	return true;
	
}


//given so no time is needed.
class Tile{
	SDL_Texture *frame; //what it is
	SDL_Rect frameRect; //where it is
	public:
	Tile(SDL_Texture *newTile,SDL_Rect tileRect){
		if(newTile!=NULL)frame=newTile;
		frameRect=tileRect;
	}
	SDL_Rect getRect(){ //used to check collision with camera
		return frameRect;
	}
	
	virtual void show(SDL_Renderer *ren,SDL_Rect &camera,int x=0,int y=0){
		SDL_Rect destRect;
		//
		destRect.w=frameRect.w*2;
		destRect.h=frameRect.h*2;
		//*4 because tiles were too small
		destRect.x=x*2;//set up x and y for screen
		destRect.y=y*2;
		if(isCollided(camera,destRect)){
			destRect.x-=camera.x;//if they are collided with camera, render them according to screen
			destRect.y-=camera.y;//this was were that scrolling offset was wrong
			SDL_RenderCopy(ren,frame,&frameRect,&destRect);
		}
	}
	
	
	void destroy() {
		SDL_DestroyTexture(frame);
	}
		
};


class BackGround{
	map<int,Tile *> tiles; //texture for each tile in a neat little map
	int grid[MAPSIZE][MAPSIZE]; //actually the map (storing tiletype in a 2d array)
	//used to load the tile type to the map (needed so a map can be built with these)
	void addtile(int type,Tile *t){
		if(tiles.count(type)==0){
			cout << "adding tile: "<< type<<endl;
			tiles[type]=t;
		}
	}
	
	public:
	//this is where the actual game map is put together
	void buildMap(){
		for(int x=0;x<MAPSIZE;x++){
			for(int y=0;y<MAPSIZE;y++){
				//if(x%2==0)
				grid[x][y]=x%6; //for now I want all floors
				//else grid[x][y]=FLOOR2;
			}
		}
	}
	//check where the camera is and only render tiles within the frame of the camera :D
	void show(SDL_Renderer *ren,SDL_Rect &camera){
		for(int x=0;x<MAPSIZE;x++){
			for(int y=0;y<MAPSIZE;y++){
					tiles[grid[x][y]]->show(ren,camera,(x*TILE_WIDTH),(y*TILE_HEIGHT));
			}
		}
	}	

	
	
};
#endif
