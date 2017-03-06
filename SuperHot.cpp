#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

//game screen size in px
const int SCREEN_WIDTH =1280;
const int SCREEN_HEIGHT = 720;

//Terrain constants
const int MAPSIZE= 50;//50*64=3200X3200PX actual map size
const int TILE_HEIGHT =64;
const int TILE_WIDTH = 64;

//types of tiles taht exist
enum TileType {FLOOR1,WOOD1,WOODBORDER,WOOD2,WOOD3,FLOOR2};


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
	
	//test bounds within
	if(bot1<=top2)return false;
	if(top1>=bot2)return false;
	if(right1<=left2)return false; 
	if(left1>=right2)return false;
	//or else they are not collided
	return true;
	
}

//media manager
class MediaManager {
	map<string,SDL_Texture *> images;
	//map<strin,SDL_MUSIC *> sounds;
	public:
	SDL_Texture *load(SDL_Renderer *ren, string imagePath){
		if(images.count(imagePath.c_str())==0){
			cout << "loading image:  "<< imagePath<<endl;
			SDL_Surface *bmp = SDL_LoadBMP(imagePath.c_str());
			if(bmp!=NULL){
				SDL_SetColorKey(bmp, SDL_TRUE, SDL_MapRGB(bmp->format, 0, 255, 0));
				images[imagePath]=SDL_CreateTextureFromSurface(ren,bmp);
				
			}
			SDL_FreeSurface(bmp);
		}
		return images[imagePath];
	}

};

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
	
	//SOMETHING WRONG HERE
	virtual void show(SDL_Renderer *ren,SDL_Rect &camera,int x=0,int y=0){
		SDL_Rect destRect;
		destRect=frameRect;
		destRect.x=x-camera.x;
		destRect.y=y-camera.y;
		//if it is within the camera we want to show it. but it isnt quite working.
		if(isCollided(camera,destRect)){
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
	
	public:
	//used to load the tile type to the map (needed so a map can be built with these)
	void addtile(int type,Tile *t){
		if(tiles.count(type)==0){
			cout << "adding tile: "<< type<<endl;
			tiles[type]=t;
		}
	}
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

class AnimationFrame {
	SDL_Texture *frame; 
	int time; // ms
	int w,h;//width & heigt of texture
	SDL_Rect frameRect;
	public:
	//Constructor to be used with Media Manager
	AnimationFrame(SDL_Texture *newFrame,SDL_Rect spriteRect, int newTime=100){
		if(newFrame!=NULL)
			frame=newFrame;
		frameRect = spriteRect;
		time = newTime;
	}
	
	//shows frame at location X,Y
	virtual void show(SDL_Renderer *ren,SDL_Rect &camera, int x=0, int y=0){
		SDL_Rect  dest; //Destination is offset on screen placement
		dest.x=x-camera.x; dest.y=y-camera.y;
		dest.h=frameRect.h; dest.w=frameRect.w;
		//ren, frame, sprite(rect), output(rect)
		SDL_RenderCopy(ren, frame, &frameRect, &dest);
	}	
	int getTime() {
		return time;
	}
	void destroy() {
		SDL_DestroyTexture(frame);
	}
};
class Animation {
	protected:
	vector<AnimationFrame *> frames;
	int totalTime;
	public:
	Animation(){
		totalTime = 0;
	}

	void addFrame(AnimationFrame *c) {
		frames.push_back(c);
		totalTime += c->getTime();
	}

	virtual void show(SDL_Renderer *ren,SDL_Rect &camera, int time,int x=0,int y=0){
		int aTime = time % totalTime;
		int tTime = 0;
		unsigned int i;
		for (i = 0; i <frames.size(); i++){
			tTime += frames[i]->getTime();
			if (aTime <= tTime)break;
		}
		frames[i]->show(ren,camera,x,y);
	}	
	virtual void destroy() {
		for (unsigned int i = 0; i < frames.size(); i++)
			frames[i]->destroy();
	}
};

 
class Sprite:public Animation{
	public:
	float x,dx,ax;
	float y,dy,ay;
	void setPos(float newX, float newY){
		x=newX;
		y=newY;
	}
	void setVelocity(float newdX, float newdY){
		dx=newdX;
		dy=newdY;
	}
	void setXVel(float newdX){
		dx = newdX;
	}
	void changePos(){
		x += dx;
		y += dy;
	}
	Sprite(){
		x=150;
		y=150;
		dx=0;
		dy=0;
		ax=0;
		ay=0;
	}
	Sprite(float x, float y, float dx, float dy, float ax, float ay, float offx, float offy){
		this->x=x;
		this->y=y;
		this->dx=dx;
		this->dy=dy;
		this->ax=ax;
		this->ay=ay;
	}
	virtual void show(SDL_Renderer *ren,SDL_Rect &camera, int time){
		Animation::show(ren,camera,time,x,y);
	}	
	void update(){
		x += dx;
		y += dy;
	}
};

class Game{
	protected:
	SDL_Window *win;
	SDL_Renderer *ren;
	bool finished;
	int ticks;
	float dt;
	public:
	virtual void init(const char *gameName, int maxW = 640, int maxH = 480, int startX = 100, int startY =100){
		if (SDL_Init(SDL_INIT_VIDEO) != 0){
			cout << "SDL_Init Error: " << SDL_GetError() << endl;
		}
		win = SDL_CreateWindow(gameName, startX, startY, maxH, maxW, SDL_WINDOW_SHOWN);
		if (win == NULL){
			cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
			SDL_Quit();
		}
		ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == NULL){
			SDL_DestroyWindow(win);
			cout << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
			SDL_Quit();
		}
	}
	virtual void done(){
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
	}
	void run() {
		int start = SDL_GetTicks();
		int oldTicks = start;

		finished = false;
		while(!finished) {
			SDL_Event event;
			if(SDL_PollEvent(&event)) {
				if(event.type == SDL_WINDOWEVENT){
					if(event.window.event == SDL_WINDOWEVENT_CLOSE)
						finished = true;
				}
				if(event.type == SDL_KEYDOWN){
					if(event.key.keysym.sym == SDLK_ESCAPE)
						finished = true;
				}
				if(!finished) handleEvent(event);
			}
			ticks = SDL_GetTicks();
			dt = (float)(ticks-oldTicks)/1000.0;
			oldTicks = ticks;
			SDL_RenderClear(ren);
			show();
			SDL_RenderPresent(ren);
		}
		int end = SDL_GetTicks();
		cout << "FPS " << (300.0*1000.0/float(end-start)) <<endl;
	}	
	virtual void show() = 0;
	virtual void handleEvent(SDL_Event &event) = 0;
};




class myGame:public Game {
	MediaManager texHandle; //use me to construct animationFrames
	SDL_Rect camera; 
	SDL_Rect triggerBox; //made a box
	Sprite shoot; //double barreled shoot animation
	BackGround bg; //bg doesnt move & needs to be placed X,Y
	bool trigger; //time trigger
	
	public:
	void init(const char *gameName = "My Game", int maxW=640, int maxH=480, int startX=0, int startY=0) {
		Game::init(gameName,SCREEN_HEIGHT,SCREEN_WIDTH); //changed the size to fit tiles (8*8 right now)
		trigger = false;
		setRect(triggerBox,0,0,100,80);//utilized my setRect function
		setRect(camera,0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		
		loadShoot();//loads dude and shooting animation in function to save init space
		loadBackground(); 
	}
	void loadShoot(){
		SDL_Rect frameRect; //used to create sprite frames (x,y,w,h)
		setRect(frameRect,158,252,30,24);
		shoot.addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,300)); //media manager handle
		
		setRect(frameRect,216,255,30,24);
		shoot.addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,50));
		
		setRect(frameRect,245,256,30,24);
		shoot.addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,50));
	}
	
	void loadBackground(){
		SDL_Rect frameRect;
		for(int i=0;i<6;i++){
		setRect(frameRect,i*64,0,64,64);//loads all the tiles directly across the spritesheet
		bg.addtile(i,new Tile(texHandle.load(ren,"Terrain.bmp"),frameRect)); //put them in Map for bg
		}
		bg.buildMap();//put the map together
		
	}
	
	//Show method now passes camera, allows rendering of tiles within only the camera (saves fps and can  center)
	void show(){
		setCamera(shoot);
		bg.show(ren,camera);
		shoot.show(ren,camera,ticks);
		shoot.update();
		
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		if(trigger) SDL_RenderFillRect(ren, &triggerBox);
		SDL_RenderPresent(ren); 
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	}
	void handleEvent(SDL_Event &event){
		switch(event.type){
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_w){
					shoot.dy = -5;
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_a){
					shoot.dx = -5;
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_s){
					shoot.dy = 5;
					trigger = true;
				}
				if(event.key.keysym.sym == SDLK_d){
					shoot.dx = 5;
					trigger = true;
				}
				break;
			case SDL_KEYUP:
				trigger = false;
				if(event.key.keysym.sym == SDLK_w) shoot.dy = 0;				
				else if(event.key.keysym.sym == SDLK_a)	shoot.dx = 0;		
				else if(event.key.keysym.sym == SDLK_s) shoot.dy = 0;			
				else if(event.key.keysym.sym == SDLK_d)	shoot.dx = 0;
				break;
		}
	}
	
	//pass a rectangle in with dimensions to change its dimensions
	void setRect(SDL_Rect &rect,int x,int y, int w, int h){
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
	}
	
	//position the camera around an object
	void setCamera(Sprite &obj){
		this->camera.x=( obj.x +15 ) - camera.w / 2;
		this->camera.y=(obj.y + 12) - camera.h/2;
		if(camera.x<0){
			camera.x=0;
		}
		if(camera.y<0){
			camera.y=0;
		}
		if(camera.x>TILE_WIDTH*MAPSIZE-camera.w){
			camera.x=TILE_WIDTH*MAPSIZE-camera.w;
		}
		if(camera.y>TILE_HEIGHT*MAPSIZE-camera.h){
			camera.y=TILE_HEIGHT*MAPSIZE-camera.h;
		}
	}
	void done(){
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
