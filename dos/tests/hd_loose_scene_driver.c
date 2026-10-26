/* Real level-2 room, native loose-floor animation, render-only observation. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include "../src/hd_game.h"
#include <dlfcn.h>
static int frames,started,age;
static void capture(const char*label){
 char p[256];snprintf(p,sizeof(p),"build/loose-%s-hd.png",label);IMG_SavePNG(pop_hd_capture(onscreen_surface_),p);
 snprintf(p,sizeof(p),"build/loose-%s-dos.png",label);IMG_SavePNG(onscreen_surface_,p);
}
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void check_wall(void){
 SDL_Surface*n=SDL_ConvertSurfaceFormat(onscreen_surface_,SDL_PIXELFORMAT_ARGB8888,0),*h=pop_hd_capture(onscreen_surface_);int holes=0;
 for(int y=140;y<180;y++)for(int x=288;x<296;x++)if(px(n,x,y)&0xffffff){
  int lit=0;for(int j=0;j<4;j++)for(int i=0;i<4;i++)if(px(h,x*4+i,y*4+j)&0xffffff)lit++;
  if(!lit)holes++;
 }
 SDL_FreeSurface(n);if(holes){fprintf(stderr,"LOOSE black holes in adjacent wall: %d\n",holes);abort();}
}
static void check_climb(void){
 SDL_Surface*h=pop_hd_capture(onscreen_surface_);int foot=0,body=0;
 for(int y=480;y<500;y++)for(int x=1240;x<1255;x++){
  Uint32 p=px(h,x,y);if(((p>>16)&255)>160&&((p>>8)&255)>160&&(p&255)>130)foot++;
 }
 for(int y=436;y<464;y++)for(int x=1230;x<1280;x++){
  Uint32 p=px(h,x,y);if(((p>>16)&255)>160&&((p>>8)&255)>160&&(p&255)>130)body++;
 }
 if(foot||body<20){fprintf(stderr,"CLIMB floor occlusion failed: foot=%d body=%d\n",foot,body);abort();}
 fprintf(stderr,"CLIMB floor occlusion PASS foot=%d body=%d\n",foot,body);
}
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");present(r);SDL_Delay(10);frames++;
 if(!started&&frames>=5&&current_level==2&&Kid.room>=1&&drawn_room>=1){
  started=1;Kid.room=next_room=drawn_room=4;Kid.x=130;Kid.y=y_land[3];Kid.curr_row=2;Kid.curr_col=4;Kid.frame=frame_15_stand;Kid.action=0;Kid.alive=-1;
  Guard.direction=dir_56_none;Char=Kid;load_room_links();need_full_redraw=1;
  for(int row=0;row<3;row++){fprintf(stderr,"ROOM4 ");for(int c=0;c<10;c++)fprintf(stderr,"%d ",level.fg[90+row*10+c]&31);fprintf(stderr,"\n");}
 }
 if(started){age++;
  if(age==15)capture("before");
  if(age==16|| (age==30&&getenv("POP_LOOSE_STAGGER"))){for(int pos=0;pos<30;pos++)if((level.fg[90+pos]&31)==tiles_11_loose && (!getenv("POP_LOOSE_STAGGER") || pos==(age==16?17:18))){get_tile(4,pos%10,pos/10);make_loose_fall(1);fprintf(stderr,"LOOSE triggered=%d\n",pos);}}
  if(age==35)capture("falling");
  if(age==50)capture("fallen");
  if(age>=30&&age<=65)check_wall();
  if(age==65){capture("settled");fprintf(stderr,"LOOSE adjacent wall PASS\n");}
  if(age==66)need_full_redraw=1;
  if(age==70)capture("redrawn");
  if(age==72){Kid.x=194;Kid.y=y_land[3];Kid.curr_row=2;Kid.curr_col=8;Kid.direction=dir_0_right;Kid.frame=frame_135_climbing_1;Kid.action=actions_1_run_jump;Kid.alive=-1;Char=Kid;seqtbl_offset_char(seq_10_climb_up);savekid();need_full_redraw=1;}
  if(age>72&&Kid.frame>=135&&Kid.frame<=149){char label[64];snprintf(label,sizeof(label),age<95?"climb-%d":"down-%d",Kid.frame);capture(label);if(Kid.frame==140)check_climb();fprintf(stderr,"CLIMB age=%d frame=%d x=%d y=%d\n",age,Kid.frame,Kid.x,Kid.y);}
  if(age==95){Kid.frame=frame_148_climbing_14;Kid.action=actions_1_run_jump;Char=Kid;seqtbl_offset_char(seq_68_climb_down);savekid();need_full_redraw=1;}
  if(age==140){SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);}
 }
 if(frames>2500)abort();
}
