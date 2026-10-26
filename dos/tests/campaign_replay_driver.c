/* Observes ordinary replay execution. No gameplay state or RNG modifications. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include "../src/hd_game.h"
#include <SDL_image.h>
extern byte moves[];
static int last_level=-1;
static unsigned visited;
static int shadow_union, jaffar_defeated;
static unsigned entered_tick;
static void capture_frame(const char *label) {
 const char *dir=getenv("POP_FULL_CAPTURES");if(!dir)return;
 char path[4096];snprintf(path,sizeof(path),"%s/level%02d-%s.png",dir,current_level,label);
 SDL_Surface *surface=pop_hd_capture(onscreen_surface_);
 if(!surface || surface->w!=1280 || surface->h!=800 || IMG_SavePNG(surface,path)) {
  fprintf(stderr,"FULL CAPTURE FAILED %s: %s\n",path,SDL_GetError());quit(95);
 }
}
void campaign_replay_tick(void) {
 /* A diagnostic checkpoint may restore a nonzero replay tick. Match the
  * normal validator's tick-zero setup, including skipping audio waits. */
 if(is_validate_mode){skipping_replay=1;replay_seek_target=replay_seek_2_end;}
 static FILE*csv;const char*path=getenv("POP_FULL_AUDIT");if(path && !csv){csv=fopen(path,"w");if(!csv){perror(path);quit(95);}}if(csv){fprintf(csv,"%u %d %d %d %d %d %d %08x %u %d %d %d %d\n",(unsigned)curr_tick,current_level,Kid.room,Kid.x,Kid.y,Kid.frame,Kid.action,(unsigned)random_seed,(unsigned)moves[curr_tick],hitp_curr,Guard.x,Guard.frame,guardhp_curr);fflush(csv);}
 if(current_level!=last_level){
  if (last_level != -1 && current_level != last_level + 1) {
   fprintf(stderr,"FULL INVALID transition=%d->%d\n",last_level,current_level);quit(97);
  }
  visited |= 1u << current_level;
  entered_tick=curr_tick;
  fprintf(stderr,"FULL entered=%d tick=%u hp=%d room=%d seed=%08x\n",
   current_level,(unsigned)curr_tick,hitp_curr,Kid.room,(unsigned)random_seed);
  last_level=current_level;
 }
 /* Optional comparison captures use the last completed render. Do not call
  * redraw_screen here: drawing also updates shared native scratch state. */
 const char *climb_dir=getenv("POP_CLIMB_DIR");
 if(climb_dir && current_level==2 && Kid.frame>=135 && Kid.frame<=149){
  char path[4096];
  snprintf(path,sizeof(path),"%s/hd-%05u-f%d.png",climb_dir,(unsigned)curr_tick,Kid.frame);
  IMG_SavePNG(pop_hd_capture(onscreen_surface_),path);
  snprintf(path,sizeof(path),"%s/dos-%05u-f%d.png",climb_dir,(unsigned)curr_tick,Kid.frame);
  IMG_SavePNG(onscreen_surface_,path);
  fprintf(stderr,"CLIMB tick=%u room=%d x=%d y=%d frame=%d action=%d row=%d\n",(unsigned)curr_tick,Kid.room,Kid.x,Kid.y,Kid.frame,Kid.action,Kid.curr_row);
 }
 const char*scene_dir=getenv("POP_SCENE_DIR");
 if(scene_dir&&current_level==2&&curr_tick%5==0){
  char file[4096];snprintf(file,sizeof(file),"%s/hd-%05u-r%d.png",scene_dir,(unsigned)curr_tick,drawn_room);IMG_SavePNG(pop_hd_capture(onscreen_surface_),file);
  snprintf(file,sizeof(file),"%s/dos-%05u-r%d.png",scene_dir,(unsigned)curr_tick,drawn_room);IMG_SavePNG(onscreen_surface_,file);
 }
 if(curr_tick==entered_tick+40)capture_frame("gameplay");
 if(current_level==12 && united_with_shadow>0)shadow_union=1;
 /* This is set by the engine's on_guard_killed() victory event, including
  * deaths by spikes or a fall, and is consumed when the exit opens. */
 if(current_level==13 && leveldoor_open==2)jaffar_defeated=1;
 if(Kid.alive>=0 || hitp_curr<=0){
  fprintf(stderr,"FULL DEAD level=%d tick=%u room=%d x=%d y=%d\n",
   current_level,(unsigned)curr_tick,Kid.room,Kid.x,Kid.y);quit(96);
 }
}
void campaign_input(void) {}
void campaign_win(void) {
 if(getenv("POP_FULL_REQUIRE_ALL") &&
    (visited!=0x7ffe || !shadow_union || !jaffar_defeated || hitp_curr<=0 || rem_min<=0)){
  fprintf(stderr,"FULL INVALID visited=%04x shadow=%d jaffar=%d hp=%d time=%d\n",
   visited,shadow_union,jaffar_defeated,hitp_curr,rem_min);quit(98);
 }
 fprintf(stderr,"FULL WIN level=%d room=%d tick=%u hp=%d\n",
  current_level,drawn_room,(unsigned)curr_tick,hitp_curr);
 fprintf(stderr,"FULL EVENTS visited=%04x shadow=%d jaffar=%d remaining=%d:%d\n",
  visited,shadow_union,jaffar_defeated,rem_min,rem_tick);
 capture_frame("win");
 if(getenv("POP_FULL_ENDING"))end_sequence();
 quit(0);
}
void campaign_ending(void) {
 if(getenv("POP_FULL_ENDING")){
  capture_frame("ending");
  fprintf(stderr,"FULL ENDING animation and victory card PASS\n");quit(0);
 }
}
