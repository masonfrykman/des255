#ifndef HUMAN_EYES_WRP_T
#define HUMAN_EYES_WRP_T

#include <SDL3/SDL.h>

typedef struct wrp_t {
	SDL_Window* window;
	SDL_Renderer* renderer;
} HumanEye;

HumanEye* newEye(const char* imagePath);

#endif