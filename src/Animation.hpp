#ifndef ANIMATION_HPP
#define ANIMATION_HPP
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
class AnimationFrame {
	SDL_Texture *frame; 
	double x;
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
	virtual void show(SDL_Renderer *ren,SDL_Rect &camera, SDL_Point* center,float angle,int x=0, int y=0){
		SDL_Rect  dest; //Destination is offset on screen placement
		dest.x=x-camera.x; dest.y=y-camera.y;
		dest.h=frameRect.h; dest.w=frameRect.w;
		//ren, frame, sprite(rect), output(rect)
		SDL_RenderCopyEx(ren, frame, &frameRect, &dest,angle,NULL,SDL_FLIP_NONE);
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

	virtual void show(SDL_Renderer *ren,SDL_Rect &camera, SDL_Point* center,float angle,int time,int x=0,int y=0){
		
		int aTime = time % totalTime;
		int tTime = 0;
		unsigned int i;
		for (i = 0; i <frames.size(); i++){
			tTime += frames[i]->getTime();
			if (aTime <= tTime)break;
		}
		frames[i]->show(ren,camera,center,angle,x,y);
	}

	virtual void destroy() {
		for (unsigned int i = 0; i < frames.size(); i++)
			frames[i]->destroy();
	}
};
#endif
