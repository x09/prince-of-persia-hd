/* Fixture: redraw the bone tile, then exercise the original wake event. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include <dlfcn.h>
static int started,elapsed,frames,verified;static Uint32 prepared;
static void capture(SDL_Renderer*r,const char*name){
 int w,h;SDL_GetRendererOutputSize(r,&w,&h);
 SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
 if(!s||SDL_RenderReadPixels(r,NULL,s->format->format,s->pixels,s->pitch))abort();
 char p[160];snprintf(p,sizeof(p),"build/hd-bones-%s.png",name);
 if(IMG_SavePNG(s,p))abort();
 SDL_FreeSurface(s);
}
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");
 present(r);SDL_Delay(10);frames++;
 if(!started&&SDL_GetTicks()>10000&&frames>5&&current_level==3&&drawn_room>=1&&Kid.room>=1){
  started=1;prepared=SDL_GetTicks();
  int room=custom->skeleton_room,row=custom->skeleton_row,col=custom->skeleton_column;
  Kid.room=next_room=room;Kid.curr_row=row;Kid.y=y_land[row+1];Kid.x=70;Kid.curr_col=0;
  Kid.alive=-1;Guard.direction=dir_56_none;leveldoor_open=0;
  for(int c=0;c<10;c++)level.fg[(room-1)*30+row*10+c]=tiles_1_floor;
  level.fg[(room-1)*30+row*10+col]=tiles_21_skeleton;
  Char=Kid;drawn_room=room;load_room_links();need_full_redraw=1;
  fprintf(stderr,"BONES prepared room=%d tick=%u\\n",room,SDL_GetTicks());
 }
 if(started&&drawn_room==custom->skeleton_room&&Kid.room==drawn_room&&SDL_GetTicks()-prepared>3000)elapsed++;
 if(elapsed==20)capture(r,"resting");
 if(elapsed==21){
  drawn_room=Kid.room=custom->skeleton_room;leveldoor_open=1;
  Kid.curr_col=custom->skeleton_trigger_column_1;
  int pos=(drawn_room-1)*30+custom->skeleton_row*10+custom->skeleton_column;
  int before=level.fg[pos]&31;
  check_skel();
  int after=level.fg[pos]&31;
  verified=before==tiles_21_skeleton&&after==tiles_1_floor&&Guard.charid==charid_4_skeleton;
  fprintf(stderr,"BONES wake before=%d after=%d guard=%d verified=%d\n",before,after,Guard.charid,verified);
  if(!verified)abort();
 }
 if(elapsed==25)capture(r,"waking");
 if(elapsed==50)capture(r,"awake");
 if(elapsed==60){SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);}
 if(frames>1500&&!started)abort();
}
