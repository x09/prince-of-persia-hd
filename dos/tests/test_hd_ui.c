#include "../src/hd_game.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 pop_hd_enable(1);SDL_Surface*n[2];int solid[2]={0};
 for(int i=0;i<2;i++){char p[128];snprintf(p,sizeof(p),"data/KID/res%d.png",617+i);n[i]=IMG_Load(p);assert(n[i]);SDL_SetColorKey(n[i],SDL_FALSE,0);SDL_SetSurfaceBlendMode(n[i],SDL_BLENDMODE_NONE);pop_hd_note("KID.DAT",617+i);pop_hd_bind(n[i]);SDL_Surface*h=pop_hd_capture(n[i]);for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)solid[i]+=(px(h,x,y)>>24)>128;}
 assert(solid[0]>solid[1]&&solid[1]>0);
 SDL_Surface*screen=SDL_CreateRGBSurfaceWithFormat(0,10,8,24,SDL_PIXELFORMAT_RGB24);pop_hd_fill(screen,NULL,0);SDL_Rect d={0,0,0,0};pop_hd_blit(n[0],NULL,screen,&d);d=(SDL_Rect){0,0,0,0};pop_hd_blit(n[1],NULL,screen,&d);
 SDL_Surface*h=pop_hd_capture(screen),*empty=pop_hd_capture(n[1]);for(int y=0;y<empty->h;y++)for(int x=0;x<empty->w;x++)if(!(px(empty,x,y)>>24))assert(!(px(h,x,y)&0xffffff));
 SDL_Surface*bed=IMG_Load("data/hd/PV/res981.png");assert(bed);SDL_Surface*b=SDL_ConvertSurfaceFormat(bed,SDL_PIXELFORMAT_ARGB8888,0);for(int y=0;y<b->h;y++)for(int x=64*4;x<74*4;x++)assert(!(px(b,x,y)>>24));
 SDL_FreeSurface(b);SDL_FreeSurface(bed);pop_hd_free(screen);pop_hd_free(n[0]);pop_hd_free(n[1]);pop_hd_shutdown();puts("HD UI: full/empty hearts distinct, empty clears full, carpet preserves column slot: ok");return 0;
}
