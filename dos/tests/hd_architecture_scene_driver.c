/* Test-only architectural fixture. Never changes installed levels or game files. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include <dlfcn.h>
static int frames,started,age,room;
static void capture(SDL_Renderer*r,const char*label){
 int w,h;SDL_GetRendererOutputSize(r,&w,&h);SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
 if(!s||SDL_RenderReadPixels(r,NULL,s->format->format,s->pixels,s->pitch))abort();
 char path[256];snprintf(path,sizeof(path),"build/architecture-%s.png",label);if(IMG_SavePNG(s,path))abort();SDL_FreeSurface(s);
}
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");present(r);SDL_Delay(10);frames++;
 if(!started&&frames>=5&&current_level==2&&drawn_room>=1&&drawn_room<=24){
  started=1;room=drawn_room;int base=(room-1)*30;
  for(int i=0;i<30;i++){level.fg[base+i]=i/10==1?tiles_1_floor:tiles_0_empty;level.bg[base+i]=0;}
  level.fg[base+0]=level.fg[base+8]=tiles_9_bigpillar_top;
  level.fg[base+10]=level.fg[base+18]=tiles_8_bigpillar_bottom;
  level.fg[base+14]=tiles_16_level_door_left;level.fg[base+15]=tiles_17_level_door_right;
  Kid.room=room;Kid.x=110;Kid.curr_col=2;Kid.curr_row=1;Kid.y=y_land[2];Kid.frame=frame_15_stand;Kid.alive=-1;
  Guard.direction=dir_56_none;Char=Kid;next_room=room;need_full_redraw=1;
 }
 if(started){age++;if(age==15)capture(r,"closed");
  if(age==16){level.bg[(room-1)*30+14]=20;need_full_redraw=1;}
  if(age==20)capture(r,"half-open");
  if(age==21){level.bg[(room-1)*30+14]=43;need_full_redraw=1;}
  if(age==25)capture(r,"open");
  if(age==26){SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);}
 }
 if(frames>2500)abort();
}
