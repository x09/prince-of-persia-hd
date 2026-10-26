#include "../src/hd_game.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 pop_hd_enable(1);
 for(int code=32;code<=131;code++){
  char path[128];snprintf(path,sizeof(path),"data/font/res%d.png",1000+code);SDL_Surface*s=IMG_Load(path);assert(s);SDL_SetColorKey(s,SDL_TRUE,0);
  pop_hd_note("font",1000+code);pop_hd_bind(s);SDL_Surface*h=pop_hd_capture(s);assert(h!=s&&h->w==s->w*4&&h->h==s->h*4);pop_hd_free(s);
 }
 SDL_Surface*n=IMG_Load("data/font/res1065.png");assert(n);SDL_SetColorKey(n,SDL_TRUE,0);
 pop_hd_note("font",1065);pop_hd_bind(n);SDL_Surface*h=pop_hd_capture(n);assert(h!=n&&h->w==n->w*4&&h->h==n->h*4);
 int edge=0;for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){unsigned a=px(h,x,y)>>24;edge+=a>0&&a<255;}assert(edge);
 SDL_Surface*t=SDL_ConvertSurfaceFormat(n,SDL_PIXELFORMAT_ARGB8888,0);pop_hd_mono(n,t,0x123456);SDL_Surface*c=pop_hd_capture(t);
 for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)assert((px(c,x,y)>>24)==(px(h,x,y)>>24)&&(px(c,x,y)&0xffffff)==0x123456);
 pop_hd_mono(n,t,0);c=pop_hd_capture(t);for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)assert(!(px(c,x,y)&0xffffff));
 IMG_SavePNG(h,"build/hd-font-A.png");pop_hd_free(t);pop_hd_free(n);pop_hd_shutdown();puts("HD outline font: AA, unchanged metrics, mono colour/black, alpha: ok");return 0;
}
