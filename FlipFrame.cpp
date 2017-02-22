#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;
//this is to manage media!
class MediaManager {
	map<string,SDL_Texture *> images;
	//map<strin,SDL_MUSIC *> sounds;
	public:
	SDL_Texture *load(string imagePath{
		
	}
};

class AnimationFrame {
	SDL_Texture *frame;
	int time, w, h; // ms
	public:
	AnimationFrame(SDL_Texture *newFrame, int newTime = 100) {
		frame = newFrame;
		time = newTime;
	}
	AnimationFrame(SDL_Renderer *ren, const char *imagePath, int newTime = 100){
		SDL_Surface *bmp = SDL_LoadBMP(imagePath);
		if (bmp == NULL){
			cout << "SDL_LoadBMP Error: " << SDL_GetError() << endl;
			SDL_Quit();
		}
		SDL_SetColorKey(bmp, SDL_TRUE, SDL_MapRGB(bmp->format, 0, 255, 0));
		w = bmp->w;
		h = bmp->h;
		frame = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_FreeSurface(bmp);
		if (frame == NULL){
			cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
			SDL_Quit();
		}
		time = newTime;
	}	
	void show(SDL_Renderer *ren, int x=0, int y=0){
		SDL_Rect src, dest;
		dest.x=x;
		dest.y=y;
		dest.h=h;
		dest.w=w;
		src.x=0;
		src.y=0;
		src.w=w;
		src.h=h;
		SDL_RenderCopy(ren, frame, &src, &dest);
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
	virtual void show(SDL_Renderer *ren, int time){
		int aTime = time % totalTime;
		int tTime = 0;
		unsigned int i;
		for (i = 0; i <frames.size(); i++){
			tTime += frames[i]->getTime();
			if (aTime <= tTime)break;
		}
		frames[i]->show(ren);
	}	
	virtual void destroy() {
		for (unsigned int i = 0; i < frames.size(); i++)
			frames[i]->destroy();
	}
};

class Sprite : public Animation {
	float x, dx, ax, y, dy, ay;
	public:
	void set(float newX=0.0, float newY=0.0, float newDx=0.0, float newDy=0.0, float newAx=0.0, float newAy=0.0){
		x = newX; dx = newDx; ax = newAx;
		y = newY; dy = newDy; ay = newAy;
	}
	void setVelocity(float newDy, float newDx){
		dx = newDx;
		dy = newDy;
	}
	Sprite():Animation(float newX=0.0, float newY=0.0, float newDx=0.0, float newDy=0.0, float newAx=0.0, float newAy=0.0){
		set(newX, newY, newDx, newDy, newAx, newAy);
	}
	void addFrames(SDL_Renderer *ren, const char *imagePath, int count, int timePerFrame = 100) {
		for (int i = 1; i <=count; i++){
			stringstream ss;
			ss << imagePath << i << ".bmp";
			addFrame(new AnimationFrame(ren, ss.str().c_str(), timePerFrame));
		}
	}
	void show(SDL_Renderer *ren, int time) {
		Animation::show(ren, time, (int)x, (int)y);
	}	
	//Jump physics
	void update(const float &dt) {
		x +=dx*dt;
		y +=dy*dt;
		dx += ax*dt;
		dy += ay*dt;
	}	
};

class Game{
	protected:
	SDL_Window *win;
	SDL_Renderer *ren;
	int ticks;
	bool finished;
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
	Animation a;
	Sprite us;
	vector<Sprite> npcs;
	public:
	void init(const char *gameName = "My Game", int maxW=640, int maxH=480, int startX=100, int startY=100) {
		Game::init(gameName);
		a.addFrame(new AnimationFrame(ren, "hello.bmp"));
		a.addFrame(new AnimationFrame(ren, "hello2.bmp", 500));
		us.addFrames(ren, "Planet", 8);
		/*world.set(0.0, 0.0, 10.0, 10.0, 0.0, 10.0);
		for (int i = 0; i < 100; i++){
			Sprite s;
			s.addFrames(ren, "planet", 8);
			s.set(rand() %maxW, rand() %maxH, rand() % 20.0 -10.0, rand() % 20.0 - 10.0, 0, 10.0);
			npcs.push_back(s);
		}*/
	}
	void show(){
		a.show(ren, ticks);
		/*for (unsigned int i = 0; i < npcs.size(); i++){
			npcs[i].show(ren, ticks);
			npcs[i].update(dt);
		}*/
		us.show(ren, ticks);
		us.update(dt);
		/*SDL_SetRenderDrawColor(ren, 0, 0, 255, 128);
		SDL_Rect rect;
		rect.x = 20;
		rect.y = 20;
		rect.w = 100;
		rect.h = 100;0
		SDL_RenderFillRect(ren, &rect);
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 128);*/
	}
	void handleEvent(SDL_Event &event){
		if(event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == SDLK_SPACE)
					world.setVelocity(10.0, -10.0);
		}
				
	}
	void done(){
		a.destroy();
		us.destroy();
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
