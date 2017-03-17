#ifndef GAME_HPP
#define GAME_HPP
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <iostream>


class Game{
	SDL_Thread *updateThread,*renderThread;
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
		if(TTF_Init()==-1){
				cout << "TTF init errror" <<endl << "CLOSING..."<<endl;
				SDL_Quit();
			
		}
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
	static int updateGame(void* self){
		Game *g=(Game*)self;
		g->updateGame();
		return 0;
	}
	void updateGame(){
		int oldTicks = SDL_GetTicks();
		while(!finished) {
			ticks = SDL_GetTicks();
			int dticks=(ticks-oldTicks);
			float dt=(float)(dticks)/1000.0;
			update(dt);
			oldTicks = ticks;
			SDL_Delay(10);

		}
	}
	static int renderGame(void* self){
		Game *g=(Game*)self;
		g->renderGame();
		return 0;
		
	}
	void renderGame(){
		int start = SDL_GetTicks();
		int numFrames = 0;

		while(!finished) {
			++numFrames;
			ticks = SDL_GetTicks();
			SDL_RenderClear(ren);
			show(ticks);
			SDL_RenderPresent(ren);
			SDL_Delay(10);
		}
		int end=SDL_GetTicks();
		cout << "FPS: "<<(numFrames*1000)/float(end-start)<<endl;
	}
	void run() {
		int result;
		finished = false;
		updateThread=SDL_CreateThread(updateGame,"Update",this);
		renderThread=SDL_CreateThread(renderGame,"Render",this);
		while(!finished) {
			SDL_Event event;
			while(SDL_PollEvent(&event)) {
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
		
		}
			SDL_WaitThread(renderThread,&result);
			SDL_WaitThread(updateThread,&result);
	}
	virtual void update(float dt)=0;	
	virtual void show(int ticks) = 0;
	virtual void handleEvent(SDL_Event &event) = 0;
};

#endif
