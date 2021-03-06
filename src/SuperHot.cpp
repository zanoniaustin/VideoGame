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
	//important game handles
	MediaManager texHandle; //use me to construct animationFrames (only one in the entire game)
	bool trigger;
	SDL_Rect camera;
	GAMESTATE gameState;
	//ttf stuff
	TTF_Font *timeFont;
	SDL_Color timeColor;
	string currentTime;
	SDL_Surface* timetexture;
	SDL_Surface* titletexture;
	SDL_Surface* messagetexture;
	double triggerTime;
	SDL_Rect triggerBox;
	SDL_Rect titleBox;
	SDL_Rect messageBox;
	//sprites
	Player player;
	Enemy enemy;
	Bullet bullet[100];
	int shot;
	int bullets;
	BackGround bg; //bg doesnt move & needs to be placed X,Y
	int firing;
	
	//determines which tile a sprite is standing on returning x,y passed in
	void isOnTile(Sprite *obj, int &x, int &y){
		x=int((obj->x+8)/(TILE_WIDTH*4));
		if(x<0)x=0;
		if(x>50)x=50;
		y=int((obj->y+12)/(TILE_HEIGHT*4));
		if(y<0)y=0;
		if(y>50)y=50;
	}
	
	//moves the player checking bounds (sometimes is weird if running straight at a wall)	
	void movePlayer(Direction dr){
		//player bounds
		static bool prevBounds[4] = {0,0,0,0};
		player.dx=player.dy=0;
		int pTop=player.y;
		int pRight=player.x+player.w();
		int pBot=player.y+player.h();
		int pLeft=player.x;
		cout << "Player Top: "<< pTop << " Right: "<< pRight << " Bot: "<< pBot<< " Left: "<< pLeft<<endl;
		//tile & position
		int tX=0, tY=0;
		isOnTile(&player,tX,tY);
		Tile t = bg.posTile(tX,tY);
		//cout << '('<<tX<<','<<tY<<") Bounds(N"<<t.isTopBound()<<" ,E"<<t.isRightBound()<<" ,S"<<t.isBotBound()<< " ,W"<<t.isLeftBound()<<')'<<endl;
		//tile bounds
		int tTop = tY*t.h();
		int tRight = tX*t.w() +t.w();
		int tBot = tY*t.h() +t.h();
		int tLeft =tX*t.w();
		cout << "Tile Top: "<< tTop << " Right: "<< tRight << " Bot: "<< tBot<< " Left: "<< tLeft<<endl;

		
		switch(dr){
			case NN:
			if(t.isTopBound()){
				if(pTop>tTop+28 && prevBounds[dr]==false)player.dy=-2;
				else{
					cout << "topBounded!"<<endl;
					prevBounds[dr]=true;
					//player.y=pTop+10;
					player.dy=0;
				}
			}
			else{
				prevBounds[dr]=false;
				player.dy=-2;
			}
			break;
			case EE:
			if(t.isRightBound()){
				if(pRight<tRight-28&&prevBounds[dr]==false)player.dx=2;
				else{
					cout << "rightBounded!"<<endl;
					prevBounds[dr]=true;
					//player.x=tRight-player.w()-10;
					player.dx=0;
				}
			}
			else{
				prevBounds[dr]=false;
				player.dx=2;
			}
			break;
			case SS:
			if(t.isBotBound()){
				if(pBot<tBot-28&&prevBounds[dr]==false)player.dy=2;
				else{
					cout << "botBounded!"<<endl;
					prevBounds[dr]=true;
					//player.y=tBot-player.h()-10;
					player.dy=0;
				}
			}
			else{
				prevBounds[dr]=false;
				player.dy=2;
			}
			break;
			case WW:
			if(t.isLeftBound()){
				if(pLeft>tLeft+28&&prevBounds[dr]==false)player.dx=-2;
				else{
					cout << "leftBounded!"<<endl;
					prevBounds[dr]=true;
					//player.x=tLeft+10;
					player.dx=0;
					
				}
			}
			else{
				prevBounds[dr]=false;
				player.dx=-2;
			}
			break;
		}
	}
	
	public:
	void init(const char *gameName = "Super Hot", int maxW=640, int maxH=480, int startX=0, int startY=0) {
		Game::init(gameName,SCREEN_HEIGHT,SCREEN_WIDTH);
		trigger = false;
		triggerTime=0;
		timeFont = TTF_OpenFont( "../assets/8bit.TTF", 28 );
		timeColor={0,0,0};
		firing = 1;
		shot = 0;
		bullets = 0;
		setRect(triggerBox,0,0,100,80);
		setRect(titleBox,350,100,600,200);
		setRect(messageBox,300,350,800,100);

		setRect(camera,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

		gameState=MainMenu;
		createMenu();
		Mix_PlayMusic(music.music, -1);
	}
	void createMenu(){
		SDL_Texture *set = texHandle.load(ren,"../assets/DungeonTiles.bmp");
		bg.createTileSet(set);
		bg.buildMainMenu();
		string fontText = "Super Hot";
		titletexture = TTF_RenderText_Solid(timeFont,fontText.c_str(),timeColor);
		fontText="Press 'p' to Play ...";
		messagetexture = TTF_RenderText_Solid(timeFont,fontText.c_str(),timeColor);
	}
	void loadBackground(){
		SDL_Texture *set = texHandle.load(ren,"../assets/DungeonTiles.bmp");
		bg.createTileSet(set);
		bg.buildMap();//put the map together
	}

	void show(int ticks){
		bg.show(ren,camera,gameState);
		if(gameState==Playing){
			//counter for the time
			if(trigger){
				triggerTime +=.1;
				triggerTime<100? setRect(triggerBox,0,0,64,60) :setRect(triggerBox,0,0,(floor(log(triggerTime)*16)),60);
				currentTime = ToString(triggerTime);
			}
			timetexture = TTF_RenderText_Solid(timeFont,currentTime.c_str(),timeColor);
			//render time
			SDL_RenderCopy(ren,SDL_CreateTextureFromSurface(ren,timetexture),NULL,&triggerBox);
			player.showFrame(ren,camera,ticks);
			enemy.showFrame(ren,camera,ticks);
			if(shot == 1)
				for(int i = 0; i < bullets; i++)
					bullet[i].showFrame(ren,camera,ticks);
		} 
		else if (gameState==MainMenu) {
			SDL_RenderCopy(ren,SDL_CreateTextureFromSurface(ren,titletexture),NULL,&titleBox);
			SDL_RenderCopy(ren,SDL_CreateTextureFromSurface(ren,messagetexture),NULL,&messageBox);
		}

	}

	void update(float dt){
		setCamera(player);
		player.update();
		if (trigger){
			enemy.speed(player.x,player.y);
			enemy.update();
		}
		else enemy.stop();
		if(shot == 1)
			for(int i = 0; i < bullets; i++)
				bullet[i].update();
	}

	void handleEvent(SDL_Event &event){
		player.handleMyEvent(event);

		//angle calculation testing here
		static int mX = event.motion.x;
		static int mY = event.motion.y;
		if(event.type==SDL_MOUSEMOTION){
			mX = event.motion.x;
			mY = event.motion.y;
		}
		static float a,b;
		a = player.x - camera.x - mX;
		b = player.y - camera.y- mY;
		player.angle = atan2(b,a) * 180 / M_PI - 180;
		a=player.x-enemy.x;
		b=player.y-enemy.y;
		enemy.angle = atan2(b,a)*180 / M_PI;

    if(gameState==Playing){
      switch(event.type){
  			case SDL_KEYDOWN:
  				if(event.key.keysym.sym == SDLK_w){
					movePlayer(NN);
  					enemy.speed(player.x,player.y);
  					trigger = true;
  				}
  				if(event.key.keysym.sym == SDLK_a){
					movePlayer(WW);
  					enemy.speed(player.x,player.y);
  					trigger = true;
  				}
  				if(event.key.keysym.sym == SDLK_s){
					movePlayer(SS);
  					enemy.speed(player.x,player.y);
  					trigger = true;
  				}
  				if(event.key.keysym.sym == SDLK_d){
					movePlayer(EE);
  					enemy.speed(player.x,player.y);
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
  					shot = 1;
					bullet[bullets].setAngle(player.angle-90);
					bullet[bullets].setxy(player.x + 15,player.y + 15);
					bullet[bullets].loadBullet(ren,texHandle);
					bullets++;
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
    else if(gameState==MainMenu){
      switch(event.type){
  			case SDL_KEYDOWN:
  				if(event.key.keysym.sym == SDLK_p){
					player.loadPlayer(ren,texHandle);
					enemy.loadEnemy(ren,texHandle);
					loadBackground();
					gameState=Playing;
  				}
      }
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
		SDL_FreeSurface(timetexture);
		SDL_FreeSurface(titletexture);
		SDL_FreeSurface(messagetexture);
		bg.destroy();
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
