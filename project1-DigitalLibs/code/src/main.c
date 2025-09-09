#define SDL_MAIN_USE_CALLBACKS 1

#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *leftwindow = NULL;
static SDL_Renderer *leftrenderer = NULL;

static SDL_Window *rightwindow = NULL;
static SDL_Renderer *rightrenderer = NULL;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    SDL_SetAppMetadata("DES255 Project 1", "1.0.0", "dev.frykman.des255-p1");

	if(argc < 2) {
		printf("include a directory of png eyes as the argument\n");
		exit(2);
	}
    
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed! %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	// TODO: load in the list of eyes
	
    if(!SDL_CreateWindowAndRenderer("HELLO", 800, 600, 0, &leftwindow, &leftrenderer)) {
        SDL_Log("SDL window creation failed! %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	if(!SDL_CreateWindowAndRenderer("ROBOT", 800, 600, 0, &rightwindow, &rightrenderer)) {
        SDL_Log("SDL window creation failed! %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	return;
}