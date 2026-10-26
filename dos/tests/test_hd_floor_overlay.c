/* Climb occlusion must reuse floor pixels, never introduce a textured block. */
#include "../src/hd_game.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 pop_hd_enable(1);
 const int ids[]={232,350,351};
 for(int k=0;k<3;k++){
 SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,320,200,24,SDL_PIXELFORMAT_RGB24);
 char path[128];snprintf(path,sizeof(path),"data/VDUNGEON/res%d.png",ids[k]);
 SDL_Surface*tile=IMG_Load(path);assert(s&&tile);
 pop_hd_note("VDUNGEON.DAT",ids[k]);pop_hd_bind(tile);
 SDL_Surface*h=pop_hd_capture(s),*mask=pop_hd_capture(tile);
 for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){
  Uint32 color=(x>=480&&y<260)?0:((40+x%60)<<16)|((70+y%50)<<8)|(100+(x+y)%70);
  ((Uint32*)((Uint8*)h->pixels+y*h->pitch))[x]=0xff000000u|color;
 }
 SDL_Surface*before=SDL_ConvertSurface(h,h->format,0);assert(before);
 pop_hd_floor_background(s);
 SDL_Rect actor={98,58,35,15};pop_hd_fill(s,&actor,SDL_MapRGB(s->format,230,20,10));
 SDL_SetColorKey(tile,SDL_TRUE,0);SDL_Rect d={100,60,tile->w,tile->h};pop_hd_blit(tile,NULL,s,&d);
 int floor_pixels=0,air_pixels=0;
 for(int y=240;y<240+mask->h;y++)for(int x=400;x<400+mask->w+16;x++){
  int sx=x-400;if(sx>=mask->w)sx=mask->w-1;
  unsigned alpha=px(mask,sx,y-240)>>24;Uint32 old=px(before,x,y),now=px(h,x,y);
  if(!(old&0xffffff)){assert((now&0xffffff)==0xe6140a);air_pixels++;}
  else if(alpha==255){assert(now==old);floor_pixels++;}
 }
 assert(floor_pixels>500&&air_pixels>100);
 /* Clip the same overlay across both viewport corners. */
 d=(SDL_Rect){-4,-2,tile->w,tile->h};assert(!pop_hd_blit(tile,NULL,s,&d));
 d=(SDL_Rect){317,198,tile->w,tile->h};assert(!pop_hd_blit(tile,NULL,s,&d));
 SDL_FreeSurface(before);pop_hd_free(tile);pop_hd_free(s);
 }
 pop_hd_shutdown();
 puts("HD floor overlay: original surface pixels restored, no stone in air PASS");return 0;
}
