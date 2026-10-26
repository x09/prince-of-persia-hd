/* Test-only encounter, no changes to level files. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include <dlfcn.h>
static int frames,started,seen,elapsed;
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");present(r);SDL_Delay(10);frames++;
 int expected=getenv("POP_GUARD_LEVEL")?atoi(getenv("POP_GUARD_LEVEL")):2;
 if(!started&&frames>=5&&current_level==expected&&drawn_room>=1&&Kid.room>=1&&Kid.room<=24&&Kid.curr_row>=0&&Kid.curr_row<3){
  started=1;
  int room=Kid.room-1,row=Kid.curr_row;
  for(int c=0;c<10;c++){level.fg[room*30+row*10+c]=tiles_1_floor;level.bg[room*30+row*10+c]=0;}
  Kid.x=110;Kid.direction=dir_0_right;Kid.curr_col=get_tile_div_mod_m7(Kid.x);
  level.guards_tile[room]=row*10+6;level.guards_x[room]=160;
  level.guards_dir[room]=dir_FF_left;level.guards_seq_hi[room]=0;level.guards_color[room]=0;level.guards_skill[room]=3;
  char_type saved=Char;int saved_room=drawn_room;drawn_room=Kid.room;enter_guard();drawn_room=saved_room;Char=saved;need_full_redraw=1;started=1;
 }
 if(started)elapsed++;
 if(started&&elapsed%10==0&&elapsed<=100){
  int w,h;SDL_GetRendererOutputSize(r,&w,&h);SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
  char path[128];snprintf(path,sizeof(path),"build/guard-level%d-scene-%03d.png",expected,frames);
  if(s&&!SDL_RenderReadPixels(r,NULL,s->format->format,s->pixels,s->pitch))IMG_SavePNG(s,path);
  SDL_FreeSurface(s);
  if(Guard.room==Kid.room&&Guard.alive<0&&Guard.sword==sword_2_drawn)seen++;
  fprintf(stderr,"GUARD scene frame=%d guard-frame=%d sword=%d alive=%d same-room=%d\n",frames,Guard.frame,Guard.sword,Guard.alive,Guard.room==Kid.room);
 }
 if(elapsed==110||frames==1500){fprintf(stderr,"GUARD encounter observed=%d\n",seen);SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);}
}
