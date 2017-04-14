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

<<<<<<< HEAD
enum GAMESTATE {MainMenu,Playing,Paused,GameOver,Quit};

class myGame:public Game {
	MediaManager texHandle; //use me to construct animationFrames (only one in the entire game)
	SDL_Rect camera; 
	GAMESTATE gameState;
=======
class myGame:public Game {
	MediaManager texHandle; //use me to construct animationFrames (only one in the entire game)
	SDL_Rect camera;

>>>>>>> 43dd0881b2576228390c57496ba7b62d06964d2d
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

	public:
	void init(const char *gameName = "Super Hot", int maxW=640, int maxH=480, int startX=0, int startY=0) {
		Game::init(gameName,SCREEN_HEIGHT,SCREEN_WIDTH); //changed the size to fit tiles (8*8 right now)
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
		setCamera(player);
		player.update();
		enemy.update();
	}

	void handleEvent(SDL_Event &event){
		player.handleMyEvent(event);
		switch(event.type){
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_w){
					player.dy = -5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_a){
					player.dx = -5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_s){
					player.dy = 5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_d){
					player.dx = 5;
					enemy.speed(player.x,player.y);
					trigger = true;
				}
				break;
			case SDL_KEYUP:
				trigger = false;
				if(event.key.keysym.sym == SDLK_w) {
					player.dy = 0;
					enemy.stop();
				}
				else if(event.key.keysym.sym == SDLK_a)	{
					player.dx = 0;
					enemy.stop();
				}
				else if(event.key.keysym.sym == SDLK_s) {
					player.dy = 0;
					enemy.stop();
				}
				else if(event.key.keysym.sym == SDLK_d)	{
					player.dx = 0;
					enemy.stop();
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT){
          //player.angle = atan(event.motion.x/event.motion.y);
					//cout << player.angle << endl;
					if (firing == 1){
						player.frameID = firing;
						firing += 1;
					}
					else if (firing == 2){
						player.frameID = firing;
						firing -= 1;
					}
          Mix_PlayChannel(-1, music.gunshot, 0);
				}
				break;
		}
		//angle calculation testing here
		float a,b;
		float pi = 3.141592653589793;
		a = player.x - camera.x - event.motion.x;
		b = player.y - camera.y- event.motion.y;
		player.angle = atan2(b,a) * 180 /pi  -180 ;
		a=player.x-enemy.x;
		b=player.y-enemy.y;
		enemy.angle = atan2(b,a)*180 / pi;
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
