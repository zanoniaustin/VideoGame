#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <SDL2/SDL_mixer.h>

class Music {
  public:
    Mix_Music *music = NULL;
    Mix_Chunk *gunshot = NULL;

    bool load_files() {
      music = Mix_LoadMUS( "../assets/music.wav" );
      if (music == NULL) {
          return false;
      }
      gunshot = Mix_LoadWAV( "../assets/gunshot.wav" );
      if (gunshot == NULL) {
          return false;
      }
      return true;
    }

    void clean_up() {
      Mix_FreeMusic(music);
      Mix_FreeChunk(gunshot);
      Mix_CloseAudio();
    }
};

#endif
