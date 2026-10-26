#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
int main(void) {
 SDL_Surface *s=IMG_Load("build/hd-preview.png");if(!s)return 1;
 SDL_Surface *small=SDL_CreateRGBSurfaceWithFormat(0,640,360,24,SDL_PIXELFORMAT_RGB24);
 if(!small)return 1;
 if(SDL_BlitScaled(s,NULL,small,NULL))return 1;
 int result=IMG_SaveJPG(small,"build/hd-preview-small.jpg",85);
 SDL_FreeSurface(s);SDL_FreeSurface(small);return result!=0;
}
