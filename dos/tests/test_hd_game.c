#include "../src/hd_game.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
static Uint32 rgb(SDL_Surface *s,int x,int y) {
 Uint32 pixel=0;memcpy(&pixel,(Uint8*)s->pixels+y*s->pitch+x*s->format->BytesPerPixel,s->format->BytesPerPixel);
 Uint8 r,g,b;SDL_GetRGB(pixel,s->format,&r,&g,&b);return ((unsigned)r<<16)|((unsigned)g<<8)|b;
}
static void same(SDL_Surface *a,SDL_Surface *b) {
 assert(a->w==b->w && a->h==b->h);
 for(int y=0;y<a->h;++y)for(int x=0;x<a->w;++x)assert(rgb(a,x,y)==rgb(b,x,y));
}
static void scale_matches(SDL_Surface *s) {
 SDL_Surface *h=pop_hd_capture(s);assert(h->w==s->w*4 && h->h==s->h*4);
 for(int y=0;y<h->h;++y)for(int x=0;x<h->w;++x)assert(rgb(h,x,y)==rgb(s,x/4,y/4));
}
int main(void) {
 SDL_Surface *s=SDL_CreateRGBSurfaceWithFormat(0,7,5,8,SDL_PIXELFORMAT_INDEX8);
 SDL_Color palette[3]={{0,0,0,255},{80,120,160,255},{240,200,100,255}};
 SDL_SetPaletteColors(s->format->palette,palette,0,3);
 for(int y=0;y<5;++y)for(int x=0;x<7;++x)((Uint8*)s->pixels)[y*s->pitch+x]=(x+y)%3;
 SDL_Surface *a=SDL_CreateRGBSurfaceWithFormat(0,31,19,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_Surface *b=SDL_CreateRGBSurfaceWithFormat(0,31,19,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_FillRect(a,NULL,0xff203040);SDL_FillRect(b,NULL,0xff203040);
 pop_hd_enable(1);
 SDL_SetColorKey(s,SDL_TRUE,0);
 for(int i=0;i<30;++i) {
  SDL_Rect clip={i%3,i%2,27,16};SDL_SetClipRect(a,&clip);SDL_SetClipRect(b,&clip);
  SDL_Rect d={i%26-2,i%17-1,7,5},copy=d;
  assert(!SDL_BlitSurface(s,NULL,a,&d));assert(!pop_hd_blit(s,NULL,b,&copy));same(a,b);scale_matches(b);
 }
 SDL_SetClipRect(a,NULL);SDL_SetClipRect(b,NULL);
 palette[1]=(SDL_Color){180,40,60,255};SDL_SetPaletteColors(s->format->palette,palette,0,3);
 SDL_Rect d={3,4,7,5},copy=d;SDL_BlitSurface(s,NULL,a,&d);pop_hd_blit(s,NULL,b,&copy);
 same(a,b);scale_matches(b);
 SDL_Rect area={4,3,9,6};SDL_FillRect(a,&area,0xff112233);pop_hd_fill(b,&area,0xff112233);
 same(a,b);scale_matches(b);
 SDL_Surface *peel=SDL_CreateRGBSurfaceWithFormat(0,9,6,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_Rect zero={0,0,9,6};pop_hd_blit(b,&area,peel,&zero);scale_matches(peel);
 pop_hd_toggle();assert(pop_hd_capture(b)==b);pop_hd_toggle();assert(pop_hd_capture(b)!=b);
 SDL_Surface *mirrored=SDL_CreateRGBSurfaceWithFormat(0,b->w,b->h,32,SDL_PIXELFORMAT_ARGB8888);
 pop_hd_mirror(b,mirrored,0);
 SDL_Surface *mh=pop_hd_capture(mirrored),*bh=pop_hd_capture(b);
 for(int y=0;y<mh->h;++y)for(int x=0;x<mh->w;++x)assert(rgb(mh,x,y)==rgb(bh,bh->w-1-x,y));
 pop_hd_mirror(mirrored,mirrored,1);
 for(int y=0;y<mh->h;++y)for(int x=0;x<mh->w;++x)assert(rgb(mh,x,y)==rgb(bh,bh->w-1-x,bh->h-1-y));
 pop_hd_fade(mirrored,b,40);
 for(int y=0;y<mh->h;++y)for(int x=0;x<mh->w;++x) {
  Uint32 v=rgb(bh,x,y);int r=(v>>16)&255,g=(v>>8)&255,blue=v&255;
  r=r>40?r-40:0;g=g>40?g-40:0;blue=blue>40?blue-40:0;
  assert(rgb(mh,x,y)==(((unsigned)r<<16)|((unsigned)g<<8)|(unsigned)blue));
 }
 SDL_FillRect(mirrored,NULL,0xff445566);pop_hd_invalidate(mirrored);scale_matches(mirrored);
 SDL_Surface *short_scene=SDL_CreateRGBSurfaceWithFormat(0,b->w,b->h-3,32,SDL_PIXELFORMAT_ARGB8888);
 pop_hd_fill(short_scene,NULL,0xff7090b0);
 pop_hd_fill(mirrored,NULL,0xff203040);
 pop_hd_fade(mirrored,short_scene,16);mh=pop_hd_capture(mirrored);
 for(int y=0;y<mh->h;++y)for(int x=0;x<mh->w;++x)
  assert(rgb(mh,x,y)==(y<(b->h-3)*4?0x6080a0u:0x203040u));
 pop_hd_free(short_scene);
 pop_hd_free(mirrored);
 pop_hd_free(peel);pop_hd_free(s);pop_hd_free(b);SDL_FreeSurface(a);
 pop_hd_shutdown();
 puts("test_hd_game: native pixels unchanged, palette/key/clip/peel/fill/4x/F10/free: ok");
 return 0;
}
