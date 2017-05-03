#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <math.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include "Animation.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Sprite.hpp"
#include "MediaManager.hpp"
#include "Background.hpp"
#include "Music.hpp"

using namespace std;

//game screen size in px
const int SCREEN_WIDTH =1280;
const int SCREEN_HEIGHT = 720;

//mingw not recognizing to_string() as a function
//made this ToString for the time value
template <typename T>
string ToString(T val){
    stringstream stream;
    stream.precision(1 );
    stream << fixed<<val;
    return stream.str();
}


enum GAMESTATE {MainMenu,Playing,Paused,GameOver,Quit};
enum Direction { NN,EE,SS,WW};

class myGame:public Game {
	MediaManager texHandle; //use me to construct animationFrames (only one in the entire game)
	SDL_Rect camera; 
	GAMESTATE gameState;
	TTF_Font *timeFont;
	SDL_Color timeColor;
	string currentTime;
	SDL_Surface* timetexture;
	double triggerTime;

	SDL_Rect triggerBox;
	Player player;
	Enemy enemy;
	BackGround bg; //bg doesnt move & needs to be placed X,Y
	bool trigger;
	int firing;
	int button;
	
	bool isCollidedOnBound(Tile t, Sprite *obj,Direction dr){
		//set bounds
		int tileX,tileY;
		isOnTile(obj,tileX,tileY);
		int tLeft = tileX;
		int oLeft = obj->x;
		int tRight = tileX+t.w();
		int oRight= obj->x+obj->w();
		int tTop = tileY;
		int oTop = obj->y;
		int tBot = tileY+t.h();
		int oBot = obj->y+obj->h();
		//cout << "bounds: N:"<<t.isTopBound()<<" E:"<<t.isRightBound() <<" S:"<<t.isBotBound()<< " W:"<<t.isRightBound()<<endl;
		//test if r1 is outside r2 (return false if it is on any axis)
		if(oBot<=tBot && t.isBotBound() && dr==SS){
			//cout << "botBound here!"<<endl;
			return true;
		}
		if(oTop<=tTop && t.isTopBound() &&dr==NN){
			//cout << "topBound here!"<<endl;
			return true;
		}
		if(oLeft<=tLeft && t.isLeftBound() && dr==WW) {
			//cout << "leftBound here!" <<endl;
			return true;
		}
		if(oRight>=tRight && t.isRightBound() && dr==EE) {
			//cout << "rightBound here!" << endl;
			return true;
		}
		//if(tBot<=rTop && !t.isBotBound())return false;
		//if(tTop>=rBot && !t.isTopBound())return false;
		//if(tRight<=rLeft && !t.isRightBound())return false;
		//if(tLeft>=rRight && !t.isLeftBound())return false;
		return false;
	}
	void isOnTile(Sprite *obj, int &x, int &y){
		x=floor((obj->x - 8)/(TILE_WIDTH*4));
		y=floor((obj->y - 12)/(TILE_HEIGHT*4));
		
	}
	
	public:
	void init(const char *gameName = "Super Hot", int maxW=640, int maxH=480, int startX=0, int startY=0) {
		Game::init(gameName,SCREEN_HEIGHT,SCREEN_WIDTH);
		trigger = false;
		button = 0;
		triggerTime=0;
		timeFont = TTF_OpenFont( "../assets/8bit.TTF", 28 );
		timeColor={0,0,0};
		firing = 1;
		setRect(triggerBox,0,0,100,80);
		setRect(camera,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		gameState=MainMenu;
		//createMenu();
		player.loadPlayer(ren,texHandle);
		enemy.loadEnemy(ren,texHandle);
		loadBackground();
		Mix_PlayMusic(music.music, -1);
	}
	void createMenu(){
		SDL_Texture *set = texHandle.load(ren,"../assets/DungeonTiles.bmp");
		bg.createTileSet(set);
		bg.buildMainMenu();
		
		
	}
	void loadBackground(){
		SDL_Texture *set = texHandle.load(ren,"../assets/DungeonTiles.bmp");
		bg.createTileSet(set);
		bg.buildMap();//put the map together
	}

	//Show method now passes camera, allows rendering of tiles within only the camera (saves fps and can center)
	void show(int ticks){
		bg.show(ren,camera);
		player.showFrame(ren,camera,ticks);
		enemy.showFrame(ren,camera,ticks);

		if(trigger){
			triggerTime +=.1;

			triggerTime<100? setRect(triggerBox,0,0,64,60) :setRect(triggerBox,0,0,(floor(log(triggerTime)*16)),60);
			currentTime = ToString(triggerTime);
		}
		timetexture = TTF_RenderText_Solid(timeFont,currentTime.c_str(),timeColor);

		SDL_RenderCopy(ren,SDL_CreateTextureFromSurface(ren,timetexture),NULL,&triggerBox);
	}

	void update(float dt){
		//added the angle calculations in the player and enemy updates
		setCamera(player);
		player.update();
		if (trigger){
			enemy.speed(player.x,player.y);
			enemy.update();
		}
		else enemy.stop();
	}

	void handleEvent(SDL_Event &event){
		//constant updates should go first
		player.handleMyEvent(event);
		float a,b;
		a = player.x - camera.x - event.motion.x;
		b = player.y - camera.y- event.motion.y;
		player.angle = atan2(b,a) * 180 /M_PI  -180 ;
		a=player.x-enemy.x;
		b=player.y-enemy.y;
		enemy.angle = atan2(b,a)*180 / M_PI;
		
		int tilePosX,tilePosY;
		isOnTile(&player,tilePosX,tilePosY);
		if(tilePosX<0)tilePosX=0;
		if(tilePosY<0)tilePosY=0;
		if(tilePosX>50)tilePosX=50;
		if(tilePosY>50)tilePosY=50;
		//relative updates next
		switch(event.type){
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_w){
					if(player.y>=0){
						if(!isCollidedOnBound(bg.posTile(tilePosX,tilePosY),&player,NN))player.dy = -5;
						else player.dy=0;
					}
					else{
						player.y=0;
						 player.dy=0;
					 }
					//player.dy = -5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_a){
					if(player.x>=0){
						if(!isCollidedOnBound(bg.posTile(tilePosX,tilePosY),&player,WW))player.dx = -5;
						else player.dx=0;
					}
					else{
						player.x=0;
						 player.dx=0;
					 }
					//player.dx = -5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_s){
					if(player.y<(50*64)-24){
						if(!isCollidedOnBound(bg.posTile(tilePosX,tilePosY),&player,SS))player.dy = 5;
						else player.dy=0;
					}
					else player.dy=0;
					//player.dy = 5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_d){
					if(player.x<(50*64)-4){
						if(!isCollidedOnBound(bg.posTile(tilePosX,tilePosY),&player,EE))player.dx = 5;
						else player.dx=0;
					}
					else player.dx=0;
					//player.dx = 5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_a){
					player.dx = -5;
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_s){
					player.dy = 5;
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_d){
					player.dx = 5;
					trigger = true;
				}
				break;
			case SDL_KEYUP:
				trigger = false;
				if(event.key.keysym.sym == SDLK_w) {
					player.dy = 0;
				}
				else if(event.key.keysym.sym == SDLK_a)	{
					player.dx = 0;
				}
				else if(event.key.keysym.sym == SDLK_s) {
					player.dy = 0;
				}
				else if(event.key.keysym.sym == SDLK_d)	{
					player.dx = 0;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT){
				    Mix_PlayChannel(-1, music.gunshot, 0);
					Bullet b(player.x, player.y);
					//b.loadBullet(ren,texHandle);
					b.showFrame(ren,camera,ticks);
					trigger = true;
					if (firing == 1){
						player.frameID = firing;
						firing += 1;
					}
					else if (firing == 2){
						player.frameID = firing;
						firing -= 1;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) trigger = false;
				break;
		}
	}

	void setRect(SDL_Rect &rect,int x,int y, int w, int h){
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
	}

	//position the camera around an object
	void setCamera(Player &obj){
		this->camera.x=(obj.x + 15) - camera.w/2;
		this->camera.y=(obj.y + 12) - camera.h/2;
		if(camera.x<0){
			camera.x=0;
		}
		if(camera.y<0){
			camera.y=0;
		}
		if(camera.x>TILE_WIDTH*MAPSIZE*4-camera.w){
			camera.x=TILE_WIDTH*MAPSIZE*4-camera.w;
		}
		if(camera.y>TILE_HEIGHT*MAPSIZE*4-camera.h){
			camera.y=TILE_HEIGHT*MAPSIZE*4-camera.h;
		}
	}
	void done(){
		player.destroy();
		enemy.destroy();
		Game::done();
	}
};

int main(int argc, char *argv[]){
	myGame g;
	g.init();
	g.run();
	g.done();
	return 0;
}
