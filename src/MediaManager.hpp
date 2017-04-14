#ifndef MEDIAMANAGER_HPP
#define MEDIAMANAGER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <math.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <iostream>
using namespace std;
//media manager
class MediaManager {
	map<string,SDL_Texture *> images;
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
#endif
