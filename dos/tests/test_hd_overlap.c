#include "../src/hd_game.h"
#include "../tools/hd_fit.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void equal(SDL_Surface*a,SDL_Surface*b){assert(a->w==b->w&&a->h==b->h);for(int y=0;y<a->h;y++)for(int x=0;x<a->w;x++)assert(px(a,x,y)==px(b,x,y));}
int main(void){
 SDL_Surface*source=SDL_CreateRGBSurfaceWithFormat(0,32,64,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_Surface*fit=SDL_CreateRGBSurfaceWithFormat(0,240,130,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_FillRect(source,NULL,0xffffffff);SDL_FillRect(fit,NULL,0);assert(!hd_fit(source,fit));
 int left=240,right=0,top=130,bottom=0;
 for(int y=0;y<130;y++)for(int x=0;x<240;x++)if((px(fit,x,y)>>24)>128){if(x<left)left=x;if(x>right)right=x;if(y<top)top=y;if(y>bottom)bottom=y;}
 assert(abs(2*(right-left+1)-(bottom-top+1))<=2);
 SDL_FreeSurface(source);SDL_FreeSurface(fit);
 SDL_Surface*kid=IMG_Load("data/KID/res401.png");assert(kid);SDL_SetColorKey(kid,SDL_TRUE,0);
 SDL_Surface*off=SDL_CreateRGBSurfaceWithFormat(0,320,200,24,SDL_PIXELFORMAT_RGB24);
 SDL_Surface*on=SDL_CreateRGBSurfaceWithFormat(0,320,200,24,SDL_PIXELFORMAT_RGB24);
 SDL_Surface*peel=SDL_CreateRGBSurfaceWithFormat(0,kid->w,kid->h,24,SDL_PIXELFORMAT_RGB24);
 pop_hd_enable(1);pop_hd_note("KID.DAT",401);pop_hd_bind(kid);
 assert(pop_hd_capture(kid)->w==(kid->w+32)*4);
 Uint32 bg=SDL_MapRGB(off->format,45,60,75);pop_hd_fill(off,NULL,bg);pop_hd_fill(on,NULL,bg);
 pop_hd_expand_peel(peel);
 SDL_Rect area={100,70,kid->w,kid->h},zero={0,0,kid->w,kid->h};pop_hd_blit(off,&area,peel,&zero);
 SDL_Rect dest=area;pop_hd_blit(kid,NULL,off,&dest);
 SDL_Surface*h=pop_hd_capture(off);unsigned overhang=0;
 for(int y=70*4;y<(70+kid->h)*4;y++)for(int x=84*4;x<100*4;x++)if((px(h,x,y)&0xffffff)!=0x2d3c4b)overhang++;
 assert(overhang); /* Art actually extends outside the DOS sprite rectangle. */
 dest=area;pop_hd_blit(off,&area,on,&dest);equal(pop_hd_capture(off),pop_hd_capture(on));
 dest=area;pop_hd_blit(peel,NULL,off,&dest);
 h=pop_hd_capture(off);for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)assert((px(h,x,y)&0xffffff)==0x2d3c4b);
 SDL_Surface*flipped=SDL_ConvertSurface(kid,kid->format,0);pop_hd_mirror(kid,flipped,0);
 SDL_Surface*a=pop_hd_capture(kid),*b=pop_hd_capture(flipped);
 assert(a->w==b->w);for(int y=0;y<a->h;y++)for(int x=0;x<a->w;x++)assert(px(a,x,y)==px(b,a->w-1-x,y));
 pop_hd_free(flipped);pop_hd_free(kid);pop_hd_free(peel);pop_hd_free(off);pop_hd_free(on);pop_hd_shutdown();
 puts("HD geometry: isotropic fit, overhang, dirty copy, full peel restore, mirrored margin: ok");return 0;
}
