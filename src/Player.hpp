#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <math.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include "Sprite.hpp"

class Player:public Sprite{
	public:
	int frameID;
	float angle;
	Player(){
		x=300;
		y=300;
		dx=0;
		dy=0;
		angle = 0;
		frameID = 0;
	}
	Player(float x, float y, float dx, float dy){
		this->x=x;
		this->y=y;
		this->dx=dx;
		this->dy=dy;
	}

	void loadPlayer(SDL_Renderer *ren,MediaManager &texHandle){
		SDL_Rect frameRect;
		setRect(frameRect,158,252,30,24);
		this->addFrame(new AnimationFrame(texHandle.load(ren,"../assets/CharacterSprite.bmp"),frameRect,300)); //media manager handle

		setRect(frameRect,216,255,30,24);
		this->addFrame(new AnimationFrame(texHandle.load(ren,"../assets/CharacterSprite.bmp"),frameRect,50));

		setRect(frameRect,245,256,30,24);
		this->addFrame(new AnimationFrame(texHandle.load(ren,"../assets/CharacterSprite.bmp"),frameRect,50));
	}

	void showFrame(SDL_Renderer *ren, SDL_Rect &camera,int time){
		frames[frameID]->show(ren,camera,angle,x,y);
		frameID = 0;
	}

	void setRect(SDL_Rect &rect,int x,int y, int w, int h){
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
	}

	virtual void update(){
		Sprite::update();
	}

	void destroy(){
		Sprite::destroy();
	}
	//sprite can handle his own event
	void handleMyEvent(SDL_Event &event){
	}
};

class Enemy:public Sprite{
	int frameID;
	public:
	float angle;
	Enemy(){
		x=200;
		y=200;
		dx=0;
		dy=0;
		angle = 0;
	}
	Enemy(float x, float y, float dx, float dy){
		this->x=x;
		this->y=y;
		this->dx=dx;
		this->dy=dy;
	}
	void loadEnemy(SDL_Renderer *ren,MediaManager &texHandle){
		frameID = 0;
		SDL_Rect frameRect; //used to create sprite frames (x,y,w,h)
		setRect(frameRect,158,252,30,24);
		this->addFrame(new AnimationFrame(texHandle.load(ren,"../assets/CharacterSprite.bmp"),frameRect,300)); //media manager handle
		/*
		setRect(frameRect,216,255,30,24);
		this->addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,50));

		setRect(frameRect,245,256,30,24);
		this->addFrame(new AnimationFrame(texHandle.load(ren,"CharacterSprite.bmp"),frameRect,50));*/
	}
	void showFrame(SDL_Renderer *ren, SDL_Rect &camera,int time){
		frames[frameID]->show(ren,camera,angle,x,y);
	}
	void speed(int px, int py){
		//player_position.x = px;
		//player_position.y = py;
		if(px == x) dx=0;
		else if(px<x) dx=-1;
		else if(px>x) dx=1;

		if(py==y)dy=0;
		else if(py<y)dy=-1;
		else if(py>y)dy=1;
	}
	void stop(){
			dx = 0;
			dy = 0;
	}
	virtual void update(){
		Sprite::update();
	}
	void setRect(SDL_Rect &rect,int x,int y, int w, int h){
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
	}
	void destroy(){
		Sprite::destroy();
	}
};

class Bullet:public Sprite{
	int x,y;
	int frameID;
	public:
	int dx,dy;
	Bullet(){
		x = 0;
		y = 0;
		dx = 0;
		dy = 0;
		frameID = 0;
	}
	Bullet(int x, int y){
		this->x = x;
		this->y = y;
		dx = 0;
		dy = 3;
		frameID = 0;
	}
	
	void loadBullet(SDL_Renderer *ren,MediaManager &texHandle, int frameID){
		this->frameID = frameID;
		SDL_Rect frameRect; //used to create sprite frames (x,y,w,h)
		setRect(frameRect,0,0,3,6);
		this->addFrame(new AnimationFrame(texHandle.load(ren,"../assets/bullet.bmp"),frameRect,300));
	}
	
	void showFrame(SDL_Renderer *ren, SDL_Rect &camera,int time){
		frames[frameID]->show(ren,camera,angle,x,y);
	}
	
	void shoot(){
		x += dx;
		y += dy;
	}
	
	void setRect(SDL_Rect &rect,int x,int y, int w, int h){
		rect.x=x;
		rect.y=y;
		rect.w=w;
		rect.h=h;
	}
	
	void destroy(){
		Sprite::destroy();
	}
};

#endif
