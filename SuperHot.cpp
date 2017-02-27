#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

//media manager (needs work)
//seems to be working
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
	virtual void show(SDL_Renderer *ren, int x=0, int y=0){
		SDL_Rect  dest; //Destination is offset on screen placement
		dest.x=x; dest.y=y;
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

	virtual void show(SDL_Renderer *ren, int time,int x=0,int y=0){
		int aTime = time % totalTime;
		int tTime = 0;
		unsigned int i;
		for (i = 0; i <frames.size(); i++){
			tTime += frames[i]->getTime();
			if (aTime <= tTime)break;
		}
		//renderer , posX, posY, offset on texture, width on texture
		frames[i]->show(ren,x,y);
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
	virtual void show(SDL_Renderer *ren, int time){
		Animation::show(ren,time,x,y);
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
	virtual void init(const char *gameName, int maxW = 640, int maxH = 480, int startX = 640, int startY = 480){
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
	Sprite shoot; //double barreled shoot animation
	bool trigger; //time trigger
	
	public:
	void init(const char *gameName = "My Game", int maxW=640, int maxH=480, int startX=100, int startY=100) {
		Game::init(gameName);
		SDL_Rect frameRect; //used to create sprite frames (x,y,w,h)
		setRect(frameRect,158,252,30,24);
		shoot.addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,300)); //media manager handle
		
		setRect(frameRect,216,255,30,24);
		shoot.addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,50));
		
		setRect(frameRect,245,256,30,24);
		shoot.addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,50));
	}
	void show(){
		shoot.show(ren,ticks);
		shoot.update();
	}
	void handleEvent(SDL_Event &event){
		switch(event.type){
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_w)
					shoot.dy = -5;
				if(event.key.keysym.sym == SDLK_a)
					shoot.dx = -5;
				if(event.key.keysym.sym == SDLK_s)
					shoot.dy = 5;
				if(event.key.keysym.sym == SDLK_d)
					shoot.dx = 5;
				break;
			case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_w)
					shoot.dy = 0;				
				if(event.key.keysym.sym == SDLK_a)
					shoot.dx = 0;			
				if(event.key.keysym.sym == SDLK_s)
					shoot.dy = 0;				
				if(event.key.keysym.sym == SDLK_d)
					shoot.dx = 0;
				break;
		}
	}
	void setRect(SDL_Rect &rect,int x,int y, int w, int h){
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
		
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
