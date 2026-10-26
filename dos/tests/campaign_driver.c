/* Test-only input adapter. Never changes tiles, HP, character position or exits. */
#define POP_HD_INTERNAL
#include "../src/common.h"
__attribute__((constructor)) static void campaign_boot(void){if(getenv("POP_CAMPAIGN_FAST_BOOT"))is_validate_mode=1;}
extern SDL_AudioSpec *digi_audiospec;
extern void audio_callback(void*, Uint8*, int);
static void advance_audio(void){
 if(!getenv("POP_CAMPAIGN_AUDIO_CLOCK") || !digi_audiospec)return;
 SDL_PauseAudio(1);
 unsigned samples=digi_audiospec->freq*(Kid.sword?custom->fight_speed:custom->base_speed)/60;
 unsigned bytes=samples*digi_audiospec->channels*(SDL_AUDIO_BITSIZE(digi_audiospec->format)/8);
 Uint8 *pcm=malloc(bytes);if(!pcm)exit(95);
 audio_callback(NULL,pcm,bytes);free(pcm);
}
void campaign_win(void){fprintf(stderr,"CAMPAIGN WIN TRIGGER level=%d room=%d hp=%d\n",current_level,drawn_room,hitp_curr);if(recording){const char*out=getenv("POP_CAMPAIGN_REPLAY");save_recorded_replay(out?out:"build/campaign/win.p1r");recording=0;}SDL_PauseAudio(0);if(getenv("POP_CAMPAIGN_RENDER"))is_validate_mode=0;if(getenv("POP_CAMPAIGN_WIN_ONLY"))quit(0);}
void campaign_ending(void){fprintf(stderr,"CAMPAIGN ENDING ANIMATION AND VICTORY CARD PASS\n");quit(0);}
static FILE*input;static int ready,levelno,step,total,room,last_level,frames;
static unsigned seed;static char move[32];static int divergence;
static void read_next(void){
 char line[1024];ready=0;
 while(fgets(line,sizeof(line),input)){
  if(sscanf(line,"Level %d, Step %d/%d: - RNG: 0x%x, IGT: %*d:%*d, Loose: %*d, Move: '%31[^']', Room: %d",
    &levelno,&step,&total,&seed,move,&room)==6){ready=1;return;}
 }
}
static void campaign_audit(void){static FILE*f;const char*p=getenv("POP_CAMPAIGN_AUDIT");if(!p)return;if(!f)f=fopen(p,"w");if(f){fprintf(f,"%d %d %d %d %d %d %d %d %d %08x %d %d\n",curr_tick,current_level,Kid.room,Kid.x,Kid.y,Kid.frame,hitp_curr,Guard.x,guardhp_curr,random_seed,last_loose_sound,sound_flags);fflush(f);}}
void campaign_input(void){
 if(replaying){campaign_audit();return;}
 if(!input){
  const char*p=getenv("POP_CAMPAIGN_INPUT");if(!p)exit(90);
  input=fopen(p,"r");if(!input)exit(91);read_next();
  if(getenv("POP_CAMPAIGN_LEGACY"))turn_fixes_and_enhancements_on_off(0);
  random_seed=seed;seed_was_init=1;
  const char*offset=getenv("POP_CAMPAIGN_SEED_OFFSET");int shift=offset?atoi(offset):0;
  while(shift>0){random_seed=random_seed*214013u+2531011u;shift--;}
  while(shift<0){random_seed=(random_seed-2531011u)*3115528533u;shift++;}
  fprintf(stderr,"CAMPAIGN initial seed=%08x offset=%s\n",random_seed,offset?offset:"0");
 }
 if(Kid.alive>=0){fprintf(stderr,"CAMPAIGN DEAD level=%d step=%d room=%d x=%d y=%d hp=%d\n",current_level,step,Kid.room,Kid.x,Kid.y,hitp_curr);quit(96);}
 advance_audio();
 campaign_audit();
 if(getenv("POP_CAMPAIGN_FIGHT") && ((current_level==6 && Kid.room==6 && step>=130 && step<168)||(current_level==10 && Kid.room==5 && step>=350 && step<388))){
  static int ft;control_x=control_y=control_shift=0;
  if(guardhp_curr>0 && Guard.alive<0){
   int dx=(int)Guard.x-(int)Kid.x;if(abs(dx)>15 && ft%2)control_x=dx>0?1:-1;
   if(Kid.sword!=2){control_x=0;control_shift=ft%2?CONTROL_HELD:0;}
   else if(Kid.frame==161)control_shift=CONTROL_HELD;
   else if((Guard.frame>=151 && Guard.frame<=154)||Guard.frame==162||Kid.frame==167)control_y=-1;
   else if(abs(dx)<=15)control_shift=(Kid.frame==161 || ft%2)?CONTROL_HELD:0;
   if(ft%20==0)fprintf(stderr,"FIGHT t=%d step=%d kid=%d,%d f%d hp%d guard=%d f%d hp%d\n",ft,step,Kid.x,Kid.y,Kid.frame,hitp_curr,Guard.x,Guard.frame,guardhp_curr);
   ft++;frames++;if(ft>1000)quit(97);return;
  }
  if(ft){static int settle;if(settle++<12){control_y=settle%2?1:0;frames++;return;}int resume=current_level==6?168:375;if(current_level==10)ready=0;while(ready && step<resume)read_next();fprintf(stderr,"FIGHT won hp=%d x=%d resume=%d\n",hitp_curr,Kid.x,step);}
 }
 if(!getenv("POP_CAMPAIGN_RENDER"))is_validate_mode=1;
 if(!ready){
  fprintf(stderr,"CAMPAIGN input-end level=%d next=%d room=%d alive=%d frames=%d divergences=%d\n",current_level,next_level,Kid.room,Kid.alive,frames,divergence);
  if(recording && (current_level!=last_level || next_level!=last_level)){const char*out=getenv("POP_CAMPAIGN_REPLAY");save_recorded_replay(out?out:"build/campaign/input-route.p1r");}
  if((current_level==last_level+1 || next_level==last_level+1) && Kid.alive<0 && hitp_curr>0){fprintf(stderr,"CAMPAIGN LEVEL EXIT PASS %d -> %d\n",last_level,next_level);quit(0);}
  static int tail;control_x=control_y=control_shift=0;
  if(!tail){fprintf(stderr,"CAMPAIGN FINAL guardhp=%d sword=%d direction=%d door=%d\n",guardhp_curr,Kid.sword,Kid.direction,leveldoor_open);for(int r=1;r<=24;r++)for(int t=0;t<30;t++)if((level.fg[(r-1)*30+t]&31)==16)fprintf(stderr,"CAMPAIGN door room=%d tile=%d mod=%d\n",r,t,level.bg[(r-1)*30+t]);}
  if(!tail && current_level==13){for(int r=1;r<=3;r++){fprintf(stderr,"ROOM %d\n",r);for(int t=0;t<30;t++)fprintf(stderr,"%02d%s",level.fg[(r-1)*30+t]&31,t%10==9?"\n":" ");}}
  if(!tail && current_level==10){for(int r=5;r<=8;r+=3){fprintf(stderr,"ROOM %d\n",r);for(int t=0;t<30;t++)fprintf(stderr,"%02d%s",level.fg[(r-1)*30+t]&31,t%10==9?"\n":" ");}}
  if(current_level==13){control_x=-1;if(Kid.sword==2)control_y=1;else if(Kid.room==1 && Kid.x<130 && Kid.x>90)control_y=-1;else if(Kid.room==3 && Kid.y<80 && Kid.x>165)control_y=-1;else if(Kid.room==3 && Kid.y>140){control_y=-1;control_shift=CONTROL_HELD;}else if(Kid.room==3 && Kid.y>90 && Kid.x<135){control_x=0;control_y=tail%2?-1:0;}}
  if(!tail && current_level==14){for(int r=3;r<=4;r++){fprintf(stderr,"ROOM %d\n",r);for(int t=0;t<30;t++)fprintf(stderr,"%02d%s",level.fg[(r-1)*30+t]&31,t%10==9?"\n":" ");}}
  if(current_level==14){static int pressed;if(Kid.room==4 && Kid.x>185)pressed=1;control_x=tail%2?(pressed?-1:1):0;}
  if(current_level==6){control_x=tail%2?-1:0;if(Kid.room==18)control_y=tail%2?-1:0;}
  if(current_level==10){static int opener;control_x=tail%2?-1:0;if(Kid.room==11){if(Kid.curr_col>=1)opener=1;control_x=tail%2?(opener?-1:1):0;}if(Kid.room==5 && Kid.x<85 && !opener){control_x=Kid.direction<0?1:0;control_y=tail%2?-1:0;control_shift=CONTROL_HELD;}if(Kid.room==5 && !opener){static int dodge;if(!dodge && Kid.x<85 && (level.fg[10*30]&31)==11)dodge=1;if(dodge>0 && dodge<35){control_x=1;control_y=Kid.direction>=0?-1:0;control_shift=0;dodge++;}}if(Kid.room==5 && opener && Kid.y<80){static int approach;if(approach==0){control_x=1;control_y=control_shift=0;if(Kid.x>=115)approach=1;}if(approach==1){control_x=control_y=control_shift=0;if(Kid.frame==15)approach=2;}if(approach==2){control_x=-1;control_y=Kid.x<105?-1:0;control_shift=0;}}if(getenv("POP_CAMPAIGN_CUT") && tail<12){control_x=0;control_y=tail%2?1:0;}if(Kid.room==8){if(Kid.y<80 && Kid.x>170)control_y=tail%2?-1:0;if(Kid.y<80 && Kid.x<130){static int descend;if(!descend)descend=1;control_x=control_y=control_shift=0;if(descend==1 && Kid.frame==15)descend=2;if(descend==2){control_x=tail%2?-1:0;control_shift=CONTROL_HELD;if(Kid.x<90)descend=3;}if(descend==3){control_x=Kid.direction<0?1:0;if(Kid.direction>=0 && Kid.frame==15)descend=4;}if(descend==4){control_x=0;control_y=tail%2?1:0;control_shift=CONTROL_HELD;}}if(Kid.y>=80 && Kid.y<150){control_x=0;control_y=1;}if(Kid.y>150){if(Kid.sword==2){control_x=0;control_y=tail%2?1:0;}else if(level.bg[7*30+23]<42){control_x=tail%2?1:0;}else{control_x=Kid.x>125?(tail%2?-1:0):Kid.x<100?(tail%2?1:0):0;if(Kid.x>=100 && Kid.x<=125)control_y=tail%2?-1:0;}}}}
  if(current_level==10 && Kid.room==8){control_x=control_y=control_shift=0;if(Kid.y<80){static int parked;if(level.bg[7*30+23]>=42){control_x=Kid.x<165?1:0;}else if(Kid.x>170){control_x=-1;control_y=-1;}else if(!parked){if(Kid.frame==15)parked=1;}else if(level.bg[7*30+23]==0){control_x=tail%2?-1:0;control_shift=CONTROL_HELD;}else{control_x=Kid.direction<0?1:0;control_shift=CONTROL_HELD;}}else if(Kid.y<150){control_y=1;}else if(Kid.sword==2){control_y=tail%2?1:0;}else if(level.bg[7*30+23]<42){control_x=tail%2?1:0;}else if(Kid.x>130){control_x=tail%2?-1:0;}else if(Kid.x<110){control_x=tail%2?1:0;}else{control_y=tail%2?-1:0;}}
  if(getenv("POP_CAMPAIGN_SECOND_FIGHT") && current_level==10 && Kid.room==8 && Kid.y>150 && guardhp_curr>0 && Guard.alive<0){int dx=(int)Guard.x-(int)Kid.x;control_x=control_y=control_shift=0;if(Kid.sword!=2){control_x=dx>0?1:-1;}else if(Kid.frame==161)control_shift=CONTROL_HELD;else if((Guard.frame>=151 && Guard.frame<=154)||Guard.frame==162||Kid.frame==167)control_y=-1;else if(abs(dx)>15)control_x=tail%2?(dx>0?1:-1):0;else control_shift=tail%2?CONTROL_HELD:0;}
  if(tail%20==0 && current_level==10)fprintf(stderr,"CAMPAIGN BUTTON col=%d row=%d exit=%d\n",Kid.curr_col,Kid.curr_row,level.bg[7*30+23]);
  if(tail%20==0)fprintf(stderr,"CAMPAIGN TAIL level=%d room=%d x=%d y=%d frame=%d hp=%d guardhp=%d\n",current_level,Kid.room,Kid.x,Kid.y,Kid.frame,hitp_curr,guardhp_curr);
  if(++tail<1600 && Kid.alive<0)return;
  quit(92);
 }
 /* DOS copy-protection interlude is disabled in SDLPoP; skip its inputs only after the real level-1 exit. */
 while(ready && levelno==15 && current_level==2)read_next();
 if(levelno!=current_level){
  if(next_level==levelno){stop_sounds();control_x=control_y=control_shift=0;return;}
  fprintf(stderr,"CAMPAIGN ROUTE MISMATCH input=%d/%d actual=%d next=%d room=%d x=%d y=%d frame=%d alive=%d hp=%d seed=%08x expected=%08x\n",levelno,step,current_level,next_level,Kid.room,Kid.x,Kid.y,Kid.frame,Kid.alive,hitp_curr,random_seed,seed);
  if(recording)save_recorded_replay("build/campaign/input-route-failed.p1r");
  quit(93);
 }
 if(current_level!=last_level){
  fprintf(stderr,"CAMPAIGN entered level=%d room=%d frame=%d hp=%d tick=%d\n",current_level,Kid.room,Kid.frame,hitp_curr,frames);last_level=current_level;
 }
 if(step<=12||step%100==0||step==total){
  fprintf(stderr,"CAMPAIGN level=%d step=%d/%d room=%d expected-room=%d x=%d y=%d alive=%d hp=%d seed=%08x expected=%08x\n",current_level,step,total,Kid.room,room,Kid.x,Kid.y,Kid.alive,hitp_curr,random_seed,seed);
 }
 divergence+=random_seed!=seed;
 control_x=strchr(move,'L')?-1:strchr(move,'R')?1:0;
 control_y=strchr(move,'U')?-1:strchr(move,'D')?1:0;
 control_shift=strchr(move,'S')?CONTROL_HELD:CONTROL_RELEASED;
 if(strchr(move,'C')&&strchr(move,'A')){is_restart_level=1;special_move=MOVE_RESTART_LEVEL;stop_sounds();}
 const char*j=getenv("POP_CAMPAIGN_EXTRA_JUMP");if(j && step==atoi(j)){control_y=-1;control_x=-1;}
 const char*cut=getenv("POP_CAMPAIGN_CUT");if(cut && step>=atoi(cut))ready=0;
 frames++;if(ready)read_next();
 if(frames>30000)quit(94);
}
