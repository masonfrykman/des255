#define SDL_HINT_APP_NAME "HELLO ROBOT"
#define SDL_MAIN_USE_CALLBACKS 1
#define P1_MAX_EYES 32

#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "HumanEye.h"

static HumanEye* windows[P1_MAX_EYES];
static int eyeCount = 0;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	// App Setup
    SDL_SetAppMetadata("DES255 Project 1", "1.0.0", "dev.frykman.des255-p1");
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "HELLO ROBOT");

	if(argc < 2) {
		printf("include a directory of png eyes as the argument\n");
		exit(1);
	}
    
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed! %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	for(size_t i = 0; i < P1_MAX_EYES; i++) {
		windows[i] = NULL;
	}

	// Load in the list of eyes,
	//		For each eye, make a window containing said eye.
	char directory[2048];
	strncpy(directory, argv[1], 2047);
	strcat(directory, "list-of-eyes.txt");

	FILE* eyesList = fopen(directory, "r");
	if(eyesList == NULL) {
		printf("couldn't open the list of eyes.\n");
		exit(1);
	}

	char* line;
	size_t lineLength;
	while(eyeCount != P1_MAX_EYES && getline(&line, &lineLength, eyesList) != -1) {
		if(line[strlen(line) - 1] == '\n') { // clean the string end (it breaks newEye)
			line[strlen(line) - 1] = '\0';
		}

		// create the path of the image
		char nD[2048];
		strncpy(nD, argv[1], 2047);
		strcat(nD, line);

		// Make the eye window
		HumanEye* candidate = newEye(nD);
		if(candidate == NULL) continue;

		// Append to the list of windows for cleanup later
		windows[eyeCount] = candidate;
		eyeCount++;
	}

	printf("done loading %d eyes.\n", eyeCount);
/*
    if(!SDL_CreateWindowAndRenderer("HELLO", 800, 600, 0, &leftwindow, &leftrenderer)) {
        SDL_Log("SDL window creation failed! %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	if(!SDL_CreateWindowAndRenderer("ROBOT", 800, 600, 0, &rightwindow, &rightrenderer)) {
        SDL_Log("SDL window creation failed! %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
*/
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	if(event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	if(event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	// Cleanup the eyes
	for(int i = 0; i < eyeCount; i++) {
		SDL_DestroyWindow(windows[i]->window);
		free(windows[i]);
	}
}