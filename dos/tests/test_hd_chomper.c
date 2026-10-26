/* DOS mono coloring must not flatten the HD blood material. */
#include "../src/hd_game.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 pop_hd_enable(1);
 for(int palace=0;palace<2;palace++)for(int id=301;id<=323;id++){
  const char*stem=palace?"VPALACE":"VDUNGEON";char path[256];snprintf(path,sizeof(path),"data/%s/res%d.png",stem,id);
  SDL_Surface*n=IMG_Load(path);assert(n);char dat[32];snprintf(dat,sizeof(dat),"%s.DAT",stem);pop_hd_note(dat,id);pop_hd_bind(n);
  SDL_Surface*h=pop_hd_capture(n);assert(h&&h->w==n->w*4&&h->h==n->h*4);
  snprintf(path,sizeof(path),"data/hd/%s/res%d.png",stem,id);
  SDL_Surface*art=IMG_Load(path);assert(art);
  SDL_Surface*expected=SDL_ConvertSurfaceFormat(art,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(art);assert(expected);
  assert(h->w==expected->w&&h->h==expected->h);
  for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)if(px(h,x,y)!=px(expected,x,y)){
   fprintf(stderr,"HD chomper: %s differs from source-tree artwork; check build/data\n",path);return 1;
  }SDL_FreeSurface(expected);
  if(id>=314){
   SDL_Surface*target=SDL_ConvertSurfaceFormat(n,SDL_PIXELFORMAT_ARGB8888,0);assert(target);
   pop_hd_mono(n,target,0xff00ff);SDL_Surface*colored=pop_hd_capture(target);int red=0,variation=0;Uint32 previous=0;
   for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){
    Uint32 p=px(h,x,y);assert(p==px(colored,x,y));
    if((p>>24)>100){int r=(p>>16)&255,g=(p>>8)&255,b=p&255;if(r>g*1.4&&r>b*1.2)red++;if(previous&&previous!=(p&0xffffff))variation++;previous=p&0xffffff;}
   }if(id<=318)assert(red>8&&variation>8);pop_hd_free(target);
  }pop_hd_free(n);
 }
 /* Ordinary mono resources must still receive their requested tint. */
 SDL_Surface*n=IMG_Load("data/VDUNGEON/res301.png"),*target=SDL_ConvertSurfaceFormat(n,SDL_PIXELFORMAT_ARGB8888,0);
 pop_hd_note("VDUNGEON.DAT",301);pop_hd_bind(n);pop_hd_mono(n,target,0x123456);SDL_Surface*h=pop_hd_capture(target);
 for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)assert((px(h,x,y)&0xffffff)==0x123456);
 pop_hd_free(n);pop_hd_free(target);pop_hd_shutdown();puts("HD chomper: both themes, 46 bindings, blood color and ordinary mono PASS");return 0;
}
