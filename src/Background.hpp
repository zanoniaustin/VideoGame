#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include "MediaManager.hpp"

using namespace std;

//Terrain constants
const int MAPSIZE= 50;//50*64=3200X3200PX actual map size
const int TILE_HEIGHT =16;
const int TILE_WIDTH = 16;

//types of tiles that exist
enum TileType {
	//Row 1
	Floor_TL,//0
	Floor_TM,
	Floor_TM1,
	FLoor_TM2,
	Floor_TR,
	Isle_TL,//5
	Isle_TM,
	Isle_TR,
	Stair_Floor,
	Stair_Shadow,
	Stair,//10
	Stair1,
	Pool_TL,
	Pool_TM,
	Pool_TR,
	Pool_ITL,//15
	Pool_ITR,
	Pool_Isle_Vert_top,
	Pool_Isle_TL,
	Pool_Isle_TM,
	Pool_Isle_TR,//20
	
	//ROW 2
	Floor_L,//21
	Floor_M,
	Floor_M1,
	Floor_M2,
	Floor_R,//25
	Isle_L,
	BlankTile,
	Isle_R,
	Floor_Shadow,
	Floor_ITL,//30
	Floor_ITR,
	Stair_Shadow1,
	Pool_L,
	Pool_M,
	Pool_R,//35
	Pool_IBL,
	Pool_IBR,
	Pool_Isle_Vert_bot,
	Pool_Isle_L,
	Pool_Isle,//40
	Pool_Isle_R,
	
	//ROW 3
	Floor_L1,
	Floor_M3,
	Floor_M4,
	Floor_M5,//45
	Floor_R1,
	Isle_BL,
	Isle_BM,
	Isle_BR,
	Floor_Shadow_b,//50
	Floor_IBL,
	Floor_IBR,
	BLackTile,
	Pool_BL,
	Pool_BM,//55
	Pool_BR,
	Waterfall,
	Fall_L,
	Stream_vert,
	Pool_Isle_BL,//60
	Pool_Isle_BM,
	Pool_Isle_BR,
	
	//Row 4
	Floor_L2,
	Floor_M6,
	Floor_M7,//65
	Floor_M8,
	Floor_End_t,
	Floor_End_l,
	Floor_End_r,
	OpenDoor,//70
	WoodDoor,
	MetalDoor,
	Stream_TL,
	Stream_TM,
	Stream_TR,//75
	Stream_BL,
	Stream_BM,
	Stream_BR,
	Wall1,
	Wall2,//80
	BlankTile2,
	BlankTile3,
	
	//Row 5
	Floor_BL,
	Floor_BM,
	Floor_BM1,//85
	Floor_BM2,
	Floor_BR,
	Floor_End_b,
	Wall3,
	Floor_end,//90
	Gutter_open,
	Gutter_close,
	Gutter_Metal,
	Stream_end_t,
	Stream_end_b,//95
	Stream_end_l,
	Stream_end_r,
	Pool_Isle_end_l,
	Pool_Isle_end_r,
	BlankTile4,//100
	Wall_end
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
		destRect.w=frameRect.w*4;
		destRect.h=frameRect.h*4;
		//*4 because tiles were too small
		destRect.x=x*4;//set up x and y for screen
		destRect.y=y*4;
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
	
	public:
	void addtile(int type,Tile *t){
		if(tiles.count(type)==0){
			//cout << "adding tile: "<< type<<endl;
			tiles[type]=t;
		}
	}
	void createTileSet(SDL_Texture *set){
		SDL_Rect tileRect;
		int row=0;
		for(int i=0;i<101+1;i++){
			if(i%20==0&&i!=0)row++;
			tileRect.x=(i%20)*TILE_WIDTH;
			tileRect.y=row*TILE_HEIGHT;
			tileRect.w=TILE_WIDTH;
			tileRect.h=TILE_HEIGHT;
			this->addtile(i,new Tile(set,tileRect));
			
		}
		
	}
	//this is where the actual game map is put together
	void buildMap(){
		ifstream fin;
		int type=5;
		fin.open("../maps/First.txt");//load first map
		for(int y=0;y<MAPSIZE;y++){
			for(int x=0;x<MAPSIZE;x++){
				fin >> type;
				grid[x][y]=type; 
			}
		}
		fin.close();
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
