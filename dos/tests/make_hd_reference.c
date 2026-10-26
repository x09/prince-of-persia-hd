#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
int main(void) {
 SDL_Surface *sheet=SDL_CreateRGBSurfaceWithFormat(0,640,384,32,SDL_PIXELFORMAT_RGBA32);
 if(!sheet)return 1;
 SDL_FillRect(sheet,NULL,0);
 for(int i=0;i<8;++i) {
  char path[100];snprintf(path,sizeof(path),"data/KID/res%d.png",407+i);
  SDL_Surface *pose=IMG_Load(path);if(!pose)return 1;
  SDL_SetColorKey(pose,SDL_TRUE,0);
  SDL_Surface *rgba=SDL_ConvertSurfaceFormat(pose,SDL_PIXELFORMAT_RGBA32,0);
  SDL_FreeSurface(pose);pose=rgba;if(!pose)return 1;
  SDL_Rect to={(i%4)*160+(160-pose->w*4)/2,(i/4)*192+176-pose->h*4,pose->w*4,pose->h*4};
  if(SDL_BlitScaled(pose,NULL,sheet,&to))return 1;
  SDL_FreeSurface(pose);
 }
 int result=IMG_SavePNG(sheet,"build/hd-run-reference.png");
 SDL_FreeSurface(sheet);return result!=0;
}
