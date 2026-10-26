/* Test-only gate fixture: edits in-memory room data, never game files. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include <dlfcn.h>
static int frames,started,slot,room;
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");present(r);frames++;
 if(!started&&frames>=5&&drawn_room>=1&&drawn_room<=24&&current_level==1){
  room=drawn_room;slot=(room-1)*30+10;level.fg[slot]=tiles_4_gate;level.bg[slot]=0;
  add_trob(room,10,1);need_full_redraw=1;started=1;
  fprintf(stderr,"GATE fixture: room=%d tile=10 opening via native animation\n",room);
 }
 if(started&&frames%8==0&&frames<=56){
  int w,h;SDL_GetRendererOutputSize(r,&w,&h);SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
  char path[128];snprintf(path,sizeof(path),"build/gate-fixture-%02d.png",frames);
  if(s&&!SDL_RenderReadPixels(r,NULL,s->format->format,s->pixels,s->pitch))IMG_SavePNG(s,path);
  SDL_FreeSurface(s);fprintf(stderr,"GATE fixture frame=%d modifier=%d\n",frames,level.bg[slot]);
 }
 if(frames==80){SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);}
}
