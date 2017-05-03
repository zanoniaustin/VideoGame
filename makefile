CC = g++

OBJS = Animation.hpp Background.hpp MediaManager.hpp Player.hpp Sprite.hpp Game.hpp
OBJ_NAME = ../deploy/SuperHot.exe

INCLUDE_P = -IC:\Users\austi\SDL2-2.0.5\i686-w64-mingw32\include -IC:\Users\austi\SDL2-2.0.5\i686-w64-mingw32\include\SDL2 -IC:\Users\austi\SDL2_ttf-2.0.14\i686-w64-mingw32\include -IC:\MinGW\include -IC:\Users\austi\SDL2_mixer-2.0.1\i686-w64-mingw32\include
LIBRARY_P = -LC:\Users\austi\SDL2_ttf-2.0.14\i686-w64-mingw32\lib -LC:\Users\austi\SDL2-2.0.5\i686-w64-mingw32\lib -LC:\Users\austi\SDL2_mixer-2.0.1\i686-w64-mingw32\lib
LINKLIBS = -lSDL2_ttf -lmingw32 -lSDL2main -lSDL2.dll -lSDL2 -lSDL2_mixer


all: ../deploy/SuperHot.exe

SuperHot.o: SuperHot.cpp $(OBJS)
		$(CC) -std=gnu++11 -Wall -c SuperHot.cpp $(INCLUDE_P) -o SuperHot.o
MediaManager.o: MediaManager.cpp MediaManager.hpp
		$(CC) -std=gnu++11 -Wall -c MediaManager.cpp $(INCLUDE_P) -o MediaManager.o
../deploy/SuperHot.exe: SuperHot.o MediaManager.o
		$(CC) $^ -o $(OBJ_NAME) $(LIBRARY_P) $(LINKLIBS)
