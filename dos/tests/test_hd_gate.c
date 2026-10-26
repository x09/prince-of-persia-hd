#include "../src/hd_game.h"
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>
static unsigned px(SDL_Surface*s,int x,int y){Uint32 p=0;Uint8 r,g,b;memcpy(&p,(Uint8*)s->pixels+y*s->pitch+x*s->format->BytesPerPixel,s->format->BytesPerPixel);SDL_GetRGB(p,s->format,&r,&g,&b);return (r<<16)|(g<<8)|b;}
int main(void){
 SDL_Surface*native=SDL_CreateRGBSurfaceWithFormat(0,64,80,24,SDL_PIXELFORMAT_RGB24);
 SDL_Surface*screen=SDL_CreateRGBSurfaceWithFormat(0,64,80,24,SDL_PIXELFORMAT_RGB24);
 SDL_Surface*slices[8];for(int i=0;i<8;i++){char p[100];snprintf(p,sizeof(p),"data/VDUNGEON/res%d.png",252+i);slices[i]=IMG_Load(p);if(!slices[i])return 2;}
 pop_hd_enable(1);unsigned errors=0;
 for(int step=0;step<64;step++){
  SDL_FillRect(native,NULL,0);pop_hd_fill(screen,NULL,0);
  for(int layer=0;layer<2;layer++)for(int y=2;y<70-step;y+=8){
   SDL_Surface*s=slices[(step+y/8)%8];SDL_SetColorKey(s,layer?SDL_TRUE:SDL_FALSE,0);SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_NONE);
   SDL_Rect a={12,y,0,0},b=a;SDL_BlitSurface(s,NULL,native,&a);pop_hd_blit(s,NULL,screen,&b);
  }
  SDL_Surface*h=pop_hd_capture(screen);
  for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)if(px(h,x,y)!=px(native,x/4,y/4))errors++;
 }
 for(int i=0;i<8;i++)pop_hd_free(slices[i]);
 pop_hd_free(screen);SDL_FreeSurface(native);pop_hd_shutdown();
 printf("HD gate: 64 opening positions, opaque/keyed slices, pixel errors=%u\n",errors);return errors?1:0;
}
