/* Render each real engine chomper frame, clean and bloodied. No level files
 * are modified: this fixture exists only inside the test process. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include <dlfcn.h>
static int frames,started,age,room;
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");present(r);SDL_Delay(10);frames++;
 if(!started&&frames>=5&&current_level>0&&drawn_room>=1&&drawn_room<=24){
  started=1;room=drawn_room;int base=(room-1)*30;
  for(int i=0;i<30;i++){level.fg[base+i]=i/10==1?tiles_1_floor:tiles_0_empty;level.bg[base+i]=0;}
  level.fg[base+14]=tiles_18_chomper;Guard.direction=dir_56_none;
 }
 if(started){
  age++;int stage=(age-1)/10;
  if(stage>=10){SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);return;}
  if(age%10==8){
   int w,h;SDL_GetRendererOutputSize(r,&w,&h);SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
   if(!s||SDL_RenderReadPixels(r,NULL,s->format->format,s->pixels,s->pitch))abort();
   char path[256];snprintf(path,sizeof(path),"build/chomper-level%d-%d.png",current_level,stage);if(IMG_SavePNG(s,path))abort();SDL_FreeSurface(s);
  }
  /* Frame modifiers map to closed, closing, then three open positions. */
  const byte mods[]={2,3,1,0,4};
  level.bg[(room-1)*30+14]=mods[stage%5]|(stage>=5?0x80:0);
  Kid.room=room;Kid.x=stage>=5?120:180;Kid.curr_col=stage>=5?4:6;Kid.curr_row=1;Kid.y=y_land[2];
  Kid.frame=stage>=5?frame_178_chomped:frame_15_stand;Kid.alive=stage>=5?0:-1;
  Char=Kid;trobs_count=0;next_room=room;need_full_redraw=1;
 }
 if(frames>2500)abort();
}
