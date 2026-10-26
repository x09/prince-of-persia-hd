#include "../src/hd_game.h"
#include <SDL_image.h>
#include <stdio.h>
static unsigned pixel(SDL_Surface*s,int x,int y){Uint32 p=0;Uint8 r,g,b;memcpy(&p,(Uint8*)s->pixels+y*s->pitch+x*s->format->BytesPerPixel,s->format->BytesPerPixel);SDL_GetRGB(p,s->format,&r,&g,&b);return (r<<16)|(g<<8)|b;}
int main(void){
 unsigned failures=0;pop_hd_enable(1);
 SDL_Surface*screen=SDL_CreateRGBSurfaceWithFormat(0,48,32,24,SDL_PIXELFORMAT_RGB24);
 SDL_Surface*peel=SDL_CreateRGBSurfaceWithFormat(0,16,18,24,SDL_PIXELFORMAT_RGB24);
 for(int frame=0;frame<9;frame++){
  char p[128];snprintf(p,sizeof(p),"data/PRINCE/res%d.png",151+frame);
  SDL_Surface*n=IMG_Load(p);snprintf(p,sizeof(p),"data/hd/PRINCE/res%d.png",151+frame);
  SDL_Surface*art=IMG_Load(p);if(!n||!art)return 2;
  pop_hd_note("PRINCE.DAT",151+frame);pop_hd_bind(n);
  SDL_SetColorKey(n,SDL_FALSE,0);SDL_SetSurfaceBlendMode(n,SDL_BLENDMODE_NONE);
  pop_hd_fill(screen,NULL,0);SDL_Rect d={8,4,16,18};pop_hd_blit(n,NULL,screen,&d);
  SDL_Surface*expected=SDL_CreateRGBSurfaceWithFormat(0,64,72,24,SDL_PIXELFORMAT_RGB24);
  SDL_FillRect(expected,NULL,0);SDL_SetSurfaceBlendMode(art,SDL_BLENDMODE_BLEND);SDL_BlitSurface(art,NULL,expected,NULL);
  SDL_Surface*h=pop_hd_capture(screen);unsigned bad=0;
  for(int y=0;y<72;y++)for(int x=0;x<64;x++)if(pixel(h,x+32,y+16)!=pixel(expected,x,y))bad++;
  /* Save background, overlap with a moving hero, restore, then redraw fire. */
  SDL_Rect area={8,4,16,18},zero={0,0,16,18};pop_hd_blit(screen,&area,peel,&zero);
  pop_hd_fill(screen,&area,SDL_MapRGB(screen->format,180,40,60));
  d=area;pop_hd_blit(peel,NULL,screen,&d);
  d=area;pop_hd_blit(n,NULL,screen,&d);h=pop_hd_capture(screen);
  for(int y=0;y<72;y++)for(int x=0;x<64;x++)if(pixel(h,x+32,y+16)!=pixel(expected,x,y))bad++;
  if(bad){fprintf(stderr,"flame %d: %u incorrect background pixels\n",frame,bad);failures+=bad;}
  SDL_FreeSurface(art);SDL_FreeSurface(expected);pop_hd_free(n);
 }
 pop_hd_free(peel);pop_hd_free(screen);pop_hd_shutdown();
 printf("HD torch opaque background + overlap/peel/redraw: failures=%u\n",failures);return failures?1:0;
}
