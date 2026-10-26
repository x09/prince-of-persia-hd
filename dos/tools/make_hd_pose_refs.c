#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const int frames[2][8]={{1,2,3,4,5,6,15,109},{34,39,44,87,91,102,106,110}};
int main(void) {
 int images[256],n=0,inside=0;char line[512];
 FILE *file=fopen("src/seg006.c","r");if(!file)return 1;
 while(fgets(line,sizeof(line),file)) {
  if(strstr(line,"const frame_type frame_table_kid[]")){inside=1;continue;}
  if(!inside)continue;
  if(strstr(line,"};"))break;
  int image;if(sscanf(line," { %d,",&image)==1 && n<256)images[n++]=image;
 }
 fclose(file);if(n<111)return 1;
 for(int group=0;group<2;++group) {
  SDL_Surface *sheet=SDL_CreateRGBSurfaceWithFormat(0,1280,768,32,SDL_PIXELFORMAT_RGBA32);if(!sheet)return 1;
  SDL_FillRect(sheet,NULL,0);
  for(int i=0;i<8;++i) {
   int frame=frames[group][i],resource=images[frame]+401;char path[128];
   snprintf(path,sizeof(path),"data/KID/res%d.png",resource);
   SDL_Surface *s=IMG_Load(path);if(!s)return 1;
   SDL_SetColorKey(s,SDL_TRUE,0);
   SDL_Surface *rgba=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_RGBA32,0);SDL_FreeSurface(s);if(!rgba)return 1;
   SDL_Rect to={(i%4)*320+(320-rgba->w*4)/2,(i/4)*384+340-rgba->h*4,rgba->w*4,rgba->h*4};
   SDL_BlitScaled(rgba,NULL,sheet,&to);SDL_FreeSurface(rgba);
   printf("group=%d cell=%d frame=%d resource=%d\n",group,i,frame,resource);
  }
  char path[100];snprintf(path,sizeof(path),"build/hd-poses-%d.png",group);
  if(IMG_SavePNG(sheet,path))return 1;
  SDL_FreeSurface(sheet);
 }
 return 0;
}
