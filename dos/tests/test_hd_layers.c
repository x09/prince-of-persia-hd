#include "../src/hd_game.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 pop_hd_enable(1);
 const char*names[]={"TITLE","PRINCE","PV"};const int ids[]={53,151,953};
 for(int i=0;i<3;i++){
  char path[128];snprintf(path,sizeof(path),"data/%s/res%d.png",names[i],ids[i]);SDL_Surface*n=IMG_Load(path);assert(n);SDL_SetColorKey(n,SDL_FALSE,0);SDL_SetSurfaceBlendMode(n,SDL_BLENDMODE_NONE);
  char dat[64];snprintf(dat,sizeof(dat),"%s.DAT",names[i]);pop_hd_note(dat,ids[i]);pop_hd_bind(n);SDL_Surface*art=pop_hd_capture(n);assert(art!=n);
  SDL_Surface*dst=SDL_CreateRGBSurfaceWithFormat(0,320,200,32,SDL_PIXELFORMAT_ARGB8888);pop_hd_fill(dst,NULL,0xff234567);
  if(i)pop_hd_scene_base(dst);
  SDL_Rect at={10,10,0,0};pop_hd_blit(n,NULL,dst,&at);SDL_Surface*h=pop_hd_capture(dst);int clear=0;
  for(int y=0;y<art->h;y++)for(int x=0;x<art->w;x++)if(!(px(art,x,y)>>24)){assert(px(h,40+x,40+y)==0xff234567);clear++;}
  assert(clear);
  if(i){
   SDL_Surface*before=SDL_ConvertSurfaceFormat(h,SDL_PIXELFORMAT_ARGB8888,0);pop_hd_scene_prepare(dst);
   at=(SDL_Rect){10,10,0,0};pop_hd_blit(n,NULL,dst,&at);h=pop_hd_capture(dst);
   for(int y=0;y<h->h;y++)assert(!memcmp((Uint8*)before->pixels+y*before->pitch,(Uint8*)h->pixels+y*h->pitch,h->w*4));
   SDL_FreeSurface(before);
  }
  pop_hd_scene_base(NULL);pop_hd_free(dst);pop_hd_free(n);
 }
 SDL_Surface*n=SDL_CreateRGBSurfaceWithFormat(0,20,20,32,SDL_PIXELFORMAT_ARGB8888);pop_hd_fill(n,NULL,0xff123456);
 SDL_Rect r={3,4,10,8};pop_hd_outline(n,&r,0xffabcdef);SDL_Surface*h=pop_hd_capture(n);assert(px(h,12,16)==0xffabcdef&&px(h,20,24)==0xff123456&&px(h,2,2)==0xff123456);
 SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,20,20,32,SDL_PIXELFORMAT_ARGB8888);SDL_FillRect(s,NULL,0xff112233);
 Uint32 before[80*80];for(int y=0;y<80;y++)memcpy(before+y*80,(Uint8*)h->pixels+y*h->pitch,320);
 SDL_Rect zero={0,0,20,20};pop_hd_xor(n,s,&zero);pop_hd_xor(n,s,&zero);h=pop_hd_capture(n);
 for(int y=0;y<80;y++)assert(!memcmp(before+y*80,(Uint8*)h->pixels+y*h->pitch,320));
 pop_hd_free(n);pop_hd_free(s);pop_hd_shutdown();puts("HD layers: transparent titles/fire/hourglass, outline retention, reversible XOR: ok");return 0;
}
