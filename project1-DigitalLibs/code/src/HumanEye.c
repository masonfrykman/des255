#include "HumanEye.h"
#include <stdlib.h>
#include <string.h>
#include <SDL3_image/SDL_image.h>

HumanEye* newEye(const char* imagePath) {
    if(imagePath == NULL || strlen(imagePath) == 0) return NULL;

    // Load the eye FIRST bc we need the width & height
    SDL_Surface* eyeImg = IMG_Load(imagePath);
    if(eyeImg == NULL) return NULL;

    // Make the eye window
    HumanEye* eye = (HumanEye*)malloc(sizeof(HumanEye));
    
    // TODO: implement rolling titles
    eye->window = SDL_CreateWindow("HELLO", eyeImg->w, eyeImg->h, 0);

    // Put the eye in the window
    SDL_Surface* wSurface = SDL_GetWindowSurface(eye->window);
    SDL_BlitSurface(eyeImg, NULL, wSurface, NULL);
    SDL_UpdateWindowSurface(eye->window);

    return eye;
}