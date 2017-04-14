#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <math.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include "Animation.hpp"

using namespace std;
class Sprite:public Animation{
	public:
	float x,dx,ax;
	float y,dy,ay;
	float angle;
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
	virtual void show(SDL_Renderer *ren,SDL_Rect &camera,float angle, int time, int x, int y){
		Animation::show(ren,camera,angle,time,x,y);
	}
		
	virtual void update(){
		x += dx;
		y += dy;
	}
};
#endif
