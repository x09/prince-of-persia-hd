/* Test-only isolated encounter. Production game contains no fixture shortcuts. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include "../src/hd_persiality.h"
#include <dlfcn.h>
#include <stdlib.h>
static int test_level(void){const char*p=getenv("POP_PERSIALITY_LEVEL");return p?atoi(p):2;}
static int frames,ready,seen,finished,steps,guard_frame,guard_x,hp,letters,typing_frames;
static char_type kid;
static Uint32 scene_clock;
static int pose_copies,unique_poses;
static SDL_Texture *rendered_poses[16];
/* Defer PNG compression until playback ends: test I/O must not consume
 * the 120 ms pose intervals being measured. Keep readback/color assertions. */
static struct { SDL_Surface *surface; char path[256]; } captures[128];
static int capture_count;
static void save_captures(void){
 for(int i=0;i<capture_count;i++){
  if(IMG_SavePNG(captures[i].surface,captures[i].path))exit(3);
  SDL_FreeSurface(captures[i].surface);
 }
 capture_count=0;
}
static void fail(const char*s){fprintf(stderr,"PERSIALITY TEST FAIL: %s\n",s);exit(3);}
static int code(void){int result=0;SDL_Scancode keys[]={SDL_SCANCODE_I,SDL_SCANCODE_D,SDL_SCANCODE_D,SDL_SCANCODE_Q,SDL_SCANCODE_D};for(int i=0;i<5;i++)result=pop_persiality_key(keys[i],0,0);return result;}
static void deny(const char*s){code();if(pop_persiality_active())fail(s);}
static void key(SDL_Scancode code){SDL_Event e={0};e.type=SDL_KEYDOWN;e.key.keysym.scancode=code;e.key.keysym.sym=SDL_GetKeyFromScancode(code);SDL_PushEvent(&e);e.type=SDL_KEYUP;SDL_PushEvent(&e);}
static void capture(SDL_Renderer*r,int index){int w,h;SDL_GetRendererOutputSize(r,&w,&h);SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);char path[256];snprintf(path,sizeof(path),"build/persiality/level%d-frame-%03d.png",test_level(),index);int lw,lh;SDL_RenderGetLogicalSize(r,&lw,&lh);
 SDL_RenderSetLogicalSize(r,0,0);SDL_RenderSetViewport(r,NULL);
 SDL_Rect full={0,0,w,h};
 if(!s||SDL_RenderReadPixels(r,&full,s->format->format,s->pixels,s->pitch))fail("capture");
 SDL_RenderSetLogicalSize(r,lw,lh);
 Uint32 edge=((Uint32*)((Uint8*)s->pixels+(h/2)*s->pitch))[0];Uint8 rr,gg,bb;SDL_GetRGB(edge,s->format,&rr,&gg,&bb);
 if(w*200>h*320 && (rr||gg||bb))fail("letterbox color leaked");
 FILE *list=fopen("build/persiality/frames.tsv","a");if(list){fprintf(list,"%s %u\n",path,SDL_GetTicks()-scene_clock);fclose(list);}
 if(capture_count>=128)fail("too many captures");
 captures[capture_count].surface=s;snprintf(captures[capture_count++].path,256,"%s",path);}
int SDL_RenderCopyEx(SDL_Renderer*r,SDL_Texture*t,const SDL_Rect*src,const SDL_Rect*dst,const double angle,const SDL_Point*center,const SDL_RendererFlip flip){
 int(*copy)(SDL_Renderer*,SDL_Texture*,const SDL_Rect*,const SDL_Rect*,double,const SDL_Point*,SDL_RendererFlip)=dlsym(RTLD_NEXT,"SDL_RenderCopyEx");
 int w,h;SDL_QueryTexture(t,NULL,NULL,&w,&h);
 if(pop_persiality_active() && w==576 && h==384){
  Uint8 alpha;SDL_GetTextureAlphaMod(t,&alpha);
  if(src || alpha!=255)fail("atlas crop or crossfade returned");
  int known=0;for(int i=0;i<unique_poses;i++)if(rendered_poses[i]==t)known=1;
  if(!known){if(unique_poses>=16)fail("unexpected extra pose");rendered_poses[unique_poses++]=t;}
  ++pose_copies;
 }
 return copy(r,t,src,dst,angle,center,flip);
}
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");
 if(pop_persiality_active() && pose_copies!=1)fail("multiple/missing pose silhouettes");
 pose_copies=0;present(r);++frames;
 if(finished&&test_level()==12)return;
 if(!ready && !(current_level==test_level() && Kid.room>=1 && Kid.room<=24))key(SDL_SCANCODE_SPACE);
 if(frames<10)fprintf(stderr,"PERSIALITY fixture frame=%d level=%d room=%d drawn=%d\n",frames,current_level,Kid.room,drawn_room);
 if(!ready && frames>=5 && current_level==test_level() && Kid.room>=1 && Kid.room<=24){
  int room=Kid.room-1,row=Kid.curr_row;
  for(int c=0;c<10;c++){level.fg[room*30+row*10+c]=tiles_1_floor;level.bg[room*30+row*10+c]=0;}
  Kid.x=110;Kid.direction=getenv("POP_PERSIALITY_LEFT")?dir_FF_left:dir_0_right;Kid.curr_col=get_tile_div_mod_m7(Kid.x);Kid.sword=0;
  loadkid();seqtbl_offset_char(seq_2_stand);play_seq();savekid();
  level.guards_tile[room]=row*10+6;level.guards_x[room]=getenv("POP_PERSIALITY_LEFT")?75:160;
  level.guards_dir[room]=getenv("POP_PERSIALITY_LEFT")?dir_0_right:dir_FF_left;level.guards_seq_hi[room]=0;level.guards_color[room]=0;level.guards_skill[room]=3;
  enter_guard();need_full_redraw=1;ready=1;
  pop_hd_toggle();deny("DOS view");pop_hd_toggle();
  pop_hd_enable(0);deny("native DOS mode");pop_hd_enable(1);
  int type=custom->tbl_guard_type[test_level()],identity=Guard.charid;
  custom->tbl_guard_type[test_level()]=-1;deny("missing opponent type");
  custom->tbl_guard_type[test_level()]=4;deny("shadow type");
  custom->tbl_guard_type[test_level()]=type;
  Guard.charid=charid_1_shadow;deny("shadow identity");Guard.charid=identity;
  is_menu_shown=1;deny("menu");is_menu_shown=0;is_paused=1;deny("paused");is_paused=0;
  recording=1;deny("recording");recording=0;replaying=1;deny("replaying");replaying=0;
  int health=guardhp_curr;guardhp_curr=0;deny("dead guard");guardhp_curr=health;
  int room_save=Guard.room;Guard.room=0;deny("offscreen guard");Guard.room=room_save;
  int kid_alive=Kid.alive;Kid.alive=0;deny("dead hero");Kid.alive=kid_alive;
  int action=Kid.action;Kid.action=actions_6_hang_straight;deny("hanging");Kid.action=actions_3_in_midair;deny("midair");Kid.action=action;
  pop_persiality_key(SDL_SCANCODE_I,0,0);pop_persiality_key(SDL_SCANCODE_D,0,0);pop_persiality_key(SDL_SCANCODE_D,0,0);pop_persiality_key(SDL_SCANCODE_X,0,0);pop_persiality_key(SDL_SCANCODE_Q,0,0);pop_persiality_key(SDL_SCANCODE_D,0,0);
  if(pop_persiality_active())fail("wrong sequence");
  /* Positive run uses real key events on separate ticks while in sword
   * stance; upper-case run carries Shift just like the user's report. */
  Kid.sword=sword_2_drawn;loadkid();seqtbl_offset_char(seq_63_guard_active_after_fall);play_seq();savekid();
  if(test_level()==12){
   Guard.charid=charid_1_shadow;
   char_type before_kid=Kid,before_guard=Guard;int health=guardhp_curr,reunion=united_with_shadow;
   deny("level-12 Shadow");
   if(memcmp(&before_kid,&Kid,sizeof Kid)||memcmp(&before_guard,&Guard,sizeof Guard)||health!=guardhp_curr||reunion!=united_with_shadow)fail("Shadow story state changed");
   fprintf(stderr,"PERSIALITY TEST PASS: level-12 Shadow excluded; reunion state untouched\n");
   finished=1;SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);return;
  }
 }
 if(ready && !seen && !pop_persiality_active()){
  static const SDL_Scancode seq[]={SDL_SCANCODE_I,SDL_SCANCODE_D,SDL_SCANCODE_D,SDL_SCANCODE_Q,SDL_SCANCODE_D};
  ++typing_frames;
  if(typing_frames%3==0 && letters<5){
   fprintf(stderr,"TYPE step=%d action=%d frame=%d rows=%d/%d alive=%d/%d hp=%d/%d paused=%d menu=%d room=%d/%d drawn=%d\n",letters,Kid.action,Kid.frame,Kid.curr_row,Guard.curr_row,Kid.alive,Guard.alive,hitp_curr,guardhp_curr,is_paused,is_menu_shown,Kid.room,Guard.room,drawn_room);
   SDL_Event e={0};e.type=SDL_KEYDOWN;e.key.keysym.scancode=seq[letters++];e.key.keysym.sym=SDL_GetKeyFromScancode(e.key.keysym.scancode);
   if(getenv("POP_PERSIALITY_UPPER"))e.key.keysym.mod=KMOD_SHIFT;
   SDL_PushEvent(&e);e.type=SDL_KEYUP;SDL_PushEvent(&e);
  }
  if(upside_down)fail("Shift+I leaked into screen inversion");
  if(typing_frames>80)fail("delayed combat typing did not activate");
 }
 if(ready && pop_persiality_active()){
  if(!seen){seen=1;scene_clock=SDL_GetTicks();FILE *list=fopen("build/persiality/frames.tsv","w");if(list)fclose(list);kid=Kid;guard_x=Guard.x;guard_frame=Guard.frame;hp=guardhp_curr;}
  ++steps;
  if(memcmp(&kid,&Kid,sizeof kid))fail("hero moved during scene");
  if(guardhp_curr>0 && (Guard.x!=guard_x || Guard.frame!=guard_frame || guardhp_curr!=hp))fail("guard not frozen");
  key(SDL_SCANCODE_RIGHT);key(SDL_SCANCODE_UP);key(SDL_SCANCODE_ESCAPE);key(SDL_SCANCODE_F6);key(SDL_SCANCODE_F9);key(SDL_SCANCODE_F10);
  if(upside_down || is_menu_shown || !pop_hd_visible())fail("scene controls leaked");
  if(steps%3==0 || steps==5 || steps==20 || steps==32 || steps==48 || steps==63)capture(r,steps);
 }
 if(seen && !pop_persiality_active() && !finished){
  if(guardhp_curr || Guard.alive<0)fail("guard alive");
  if(unique_poses!=16)fail("not all sixteen poses displayed");
  if(test_level()==13 && leveldoor_open!=2)fail("Jaffar victory did not open exit");
  if(test_level()==3){
   if(Guard.direction!=dir_56_none||level.guards_tile[kid.room-1]!=0xff)fail("skeleton not dispelled persistently");
   enter_guard();if(Guard.direction!=dir_56_none)fail("skeleton resurrected on room entry");
  }
  if(memcmp(&kid,&Kid,sizeof kid))fail("hero state not restored");
  if(need_quick_save||need_quick_load||is_menu_shown)fail("buffered action");
  /* PNG capture and the software renderer are expensive; require more
   * presentations than the 10 Hz fight simulation, not a hardware FPS promise. */
  if(steps<45)fail("presentation still runs at DOS frame rate");
  Uint32 elapsed=SDL_GetTicks()-scene_clock;if(elapsed<2800 || elapsed>6000)fail("scene duration");
  finished=1;capture(r,9999);save_captures();
  fprintf(stderr,"PERSIALITY PRESENTATION: frames=%d duration_ms=%u black_borders=PASS\n",steps,elapsed);
  fprintf(stderr,"PERSIALITY TEST PASS: shadow excluded/DOS/menu/replays; input sequence; frozen hero/guard; death; restoration; blocked controls\n");
  SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);
 }
 if(frames>600)fail("scene did not finish");
}
