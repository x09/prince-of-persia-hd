/* Hidden HD-only finishing scene. GPL-3.0-or-later.
 * Deliberately unavailable during replay recording/playback: the old replay
 * format has no spell event and must never record a divergent simulation. */
#include "common.h"
#include "hd_persiality.h"
static int running, tick, progress, scene_level, scene_room, struck, scene_presented;
static Uint32 last_key_time, scene_started, impact_started;
static SDL_Surface *poses[16], *title;
static SDL_Texture *pose_textures[16], *title_texture;
static SDL_Renderer *owner;

static int opponent_allowed(void) {
 int type=custom->tbl_guard_type[current_level];
 /* The Shadow must survive for the level-12 reunion. Skeletons have no
  * ordinary death frames, so the spell dispels them instead of stabbing. */
 return (Guard.charid==charid_2_guard&&(type==0||type==1||type==3))||
  (Guard.charid==charid_4_skeleton&&type==2);
}
static int input_context(void) {
 return pop_hd_visible() && current_level>0 && current_level<14 &&
  !is_paused && !is_menu_shown && !recording && !replaying &&
  !upside_down && Kid.alive<0 && hitp_curr>0 && Kid.room==drawn_room &&
  Guard.room==drawn_room && Guard.direction!=dir_56_none && Guard.alive<0 &&
  guardhp_curr>0 && opponent_allowed() &&
  Kid.curr_row==Guard.curr_row;
}
static int eligible(void) {
 /* Combat hurt/recoil frames must not break a code typed across game ticks. */
 return input_context() && Kid.action!=actions_2_hang_climb && Kid.action!=actions_3_in_midair &&
  Kid.action!=actions_4_in_freefall && Kid.action!=actions_6_hang_straight &&
  !(Kid.frame>=217 && Kid.frame<=228);
}
static int load_art(void) {
 for(int i=0;i<16;i++){
  if(!poses[i]){char path[128];snprintf(path,sizeof(path),"data/hd/PERSIALITY/pose%02d.png",i);poses[i]=IMG_Load(locate_file(path));}
  if(!poses[i] || poses[i]->w!=576 || poses[i]->h!=384)return 0;
 }
 if(!title)title=IMG_Load(locate_file("data/hd/PERSIALITY/title.png"));
 return title!=NULL;
}
int pop_persiality_active(void){return running;}
int pop_persiality_key(SDL_Scancode code, SDL_Keymod mod,int repeat) {
 static const SDL_Scancode sequence[]={SDL_SCANCODE_I,SDL_SCANCODE_D,SDL_SCANCODE_D,SDL_SCANCODE_Q,SDL_SCANCODE_D};
 if(running)return 1;
 /* Shift/Caps Lock change letter case; never let Shift+I reach the native
  * megahit screen-inversion shortcut while entering this HD code. */
 if(code==SDL_SCANCODE_LSHIFT || code==SDL_SCANCODE_RSHIFT || code==SDL_SCANCODE_CAPSLOCK)return 0;
 if(!input_context() || (mod&(KMOD_CTRL|KMOD_ALT|KMOD_GUI))){progress=0;return 0;}
 if(repeat)return progress>0;
 Uint32 now=SDL_GetTicks();if(now-last_key_time>2500)progress=0;last_key_time=now;
 if(code==sequence[progress])++progress;else progress=(code==sequence[0]);
 /* Consume every matching prefix, not just the final key. */
 if(progress!=5)return progress>0;
 progress=0;
 if(!eligible())return 1;
 if(!load_art()){fprintf(stderr,"Persiality: missing art, activation cancelled\n");return 0;}
 running=1;tick=0;struck=0;scene_presented=0;scene_started=SDL_GetTicks();impact_started=0;scene_level=current_level;scene_room=drawn_room;
 clear_kbd_buf();memset(key_states,0,sizeof(key_states));
 need_quick_save=need_quick_load=0;
 need_full_redraw=1;
 fprintf(stderr,"Persiality: begin level=%d room=%d\n",scene_level,scene_room);
 return 1;
}
static void finish(void) {
 running=0;tick=0;progress=0;need_full_redraw=1;
 clear_kbd_buf();memset(key_states,0,sizeof(key_states));
 memset(joy_button_states,0,sizeof(joy_button_states));
 memset(joy_axis,0,sizeof(joy_axis));memset(joy_axis_max,0,sizeof(joy_axis_max));
 memset(joy_left_stick_states,0,sizeof(joy_left_stick_states));
 memset(joy_right_stick_states,0,sizeof(joy_right_stick_states));
 release_arrows();control_shift=control_shift2=CONTROL_RELEASED;
 need_quick_save=need_quick_load=0;
}
int pop_persiality_step(void) {
 if(!running)return 0;
 if(current_level!=scene_level || drawn_room!=scene_room || Kid.alive>=0){finish();return 0;}
 if(!scene_presented)return 1;
 ++tick;
 if(tick==1)play_sound(sound_33_small_potion);
 if(!struck && SDL_GetTicks()-scene_started>=1300){
  impact_started=SDL_GetTicks();
  struck=1;guardhp_delta=-guardhp_curr;hitp_delta=0;do_delta_hp();
  loadshad_and_opp();Char.alive=0;on_guard_killed();
  if(Char.charid==charid_4_skeleton){
   /* Do not use missing skeleton corpse frames or the fall/reappear path.
    * Remove its room entry as well, so revisiting cannot resurrect it. */
   level.guards_tile[Guard.room-1]=0xff;clear_char();saveshad();
  }else{
   /* Includes Jaffar: on_guard_killed above opens the final exit and
    * triggers his normal victory sound/flash before playing the corpse. */
   seqtbl_offset_char(seq_85_stabbed_to_death);play_seq();saveshad();
  }
  fprintf(stderr,"Persiality: strike hp=%d\n",guardhp_curr);
 }else if(struck&&Guard.direction!=dir_56_none){play_guard_frame();}
 if(tick==1)need_full_redraw=1;
 if(SDL_GetTicks()-scene_started>=3000){finish();fprintf(stderr,"Persiality: end\n");}
 return 1;
}
void pop_persiality_draw(SDL_Renderer *r) {
 if(!running || !poses[0] || !title || !pop_hd_visible())return;
 if(owner!=r){pop_persiality_shutdown();owner=r;}
 if(!title_texture)title_texture=SDL_CreateTextureFromSurface(r,title);
 if(!title_texture)return;
 SDL_SetTextureBlendMode(title_texture,SDL_BLENDMODE_BLEND);
 int w,h;SDL_RenderGetLogicalSize(r,&w,&h);if(w<=0||h<=0)SDL_GetRendererOutputSize(r,&w,&h);
 Uint32 elapsed=scene_presented?SDL_GetTicks()-scene_started:0;
 static const Uint32 pose_time[16]={0,120,240,360,480,600,720,840,960,1080,1200,1320,1500,1700,1900,2150};
 int cell=0;while(cell<15 && elapsed>=pose_time[cell+1])++cell;
 if(!pose_textures[cell])pose_textures[cell]=SDL_CreateTextureFromSurface(r,poses[cell]);
 if(!pose_textures[cell])return;
 SDL_SetTextureBlendMode(pose_textures[cell],SDL_BLENDMODE_BLEND);
 /* Start visible animation time after initial redraw/texture preparation. */
 if(!scene_presented){scene_started=SDL_GetTicks();scene_presented=1;}
 /* Exactly ONE padded key pose per presentation: no crossfade doubles. */
 float x=(Kid.x*2-110)/320.0f*w, y=(Kid.y+5)/200.0f*h;
 SDL_Rect dst={(int)(x-w*.133875f),(int)(y-h*.2465f),(int)(w*.26775f),(int)(h*.2465f)};
 SDL_RendererFlip flip=Kid.direction==dir_0_right?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE;
 SDL_RenderCopyEx(r,pose_textures[cell],NULL,&dst,0,NULL,flip);
 if(elapsed>=500 && elapsed<2700){
  float ratio=(float)title->w/title->h;
  SDL_Rect text={w/5,h/30,w*3/5,(int)(w*.6f/ratio)};
  if(text.h>h/6)text.h=h/6;
  SDL_RenderCopy(r,title_texture,NULL,&text);
 }
 /* Restore draw state and restrict the flash to the game rectangle. A
  * RenderClear with a leftover yellow color also paints the letterbox bars. */
 Uint32 flash_age=SDL_GetTicks()-impact_started;
 if(struck && flash_age<160){
  Uint8 red,green,blue,alpha;SDL_BlendMode mode;
  SDL_GetRenderDrawColor(r,&red,&green,&blue,&alpha);SDL_GetRenderDrawBlendMode(r,&mode);
  SDL_Rect game={0,0,w,h};
  SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(r,255,230,140,(Uint8)(105*(160-flash_age)/160));
  SDL_RenderFillRect(r,&game);
  SDL_SetRenderDrawColor(r,red,green,blue,alpha);SDL_SetRenderDrawBlendMode(r,mode);
 }
}
void pop_persiality_shutdown(void) {
 for(int i=0;i<16;i++){if(pose_textures[i])SDL_DestroyTexture(pose_textures[i]);pose_textures[i]=NULL;}
 if(title_texture)SDL_DestroyTexture(title_texture);
 title_texture=NULL;owner=NULL;
}
