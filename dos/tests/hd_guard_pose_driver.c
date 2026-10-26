/* Exhaustive render fixture: every guard frame and both facing directions.
 * Uses real loaded level resources and native object/midtable rendering. */
#define POP_HD_INTERNAL
#include "../src/common.h"
#include <dlfcn.h>
static int done,frames;
static void audit(int levelno){
 char_type saved_guard=Guard,saved_char=Char;
 int saved_mode=draw_mode;SDL_Surface*saved_target=current_target_surface;
 Guard.charid=levelno==12?charid_1_shadow:levelno==3?charid_4_skeleton:charid_2_guard;
 FILE*log=fopen("build/guard-engine-poses.tsv","a");if(!log)abort();
 Guard.room=drawn_room;Guard.x=160;Guard.y=Kid.y;Guard.curr_row=Kid.curr_row;
 Guard.curr_col=get_tile_div_mod_m7(Guard.x);Guard.sword=sword_2_drawn;Guard.alive=-1;
 current_target_surface=onscreen_surface_;draw_mode=0;
 int count=0;
 for(int page=0;page<5;page++){
  SDL_Surface*sheet=SDL_CreateRGBSurfaceWithFormat(0,1400,640,32,SDL_PIXELFORMAT_ARGB8888);
  SDL_FillRect(sheet,NULL,0xff202b38);
  for(int k=0;k<20;k++){
   int ix=page*20+k,pose=ix/2,direction=(ix&1)?0:-1;
   if(ix>=82)continue;
   int frame=pose<36?150+pose:102+pose-36;
   Char=Guard;Char.frame=frame;load_frame();
   int table=id_chtab_2_kid+(cur_frame.sword>>6);
   if(cur_frame.image==255||!chtab_addrs[table]||cur_frame.image>=chtab_addrs[table]->n_images)continue;
   SDL_Surface*body=chtab_addrs[table]->images[cur_frame.image];
   if(!body||(body->w==1&&body->h==1))continue;
   free_peels();pop_hd_fill(onscreen_surface_,NULL,SDL_MapRGB(onscreen_surface_->format,32,43,56));
   objtable_count=midtable_count=0;Guard.frame=frame;Guard.direction=direction;
   draw_guard();
   for(int i=0;i<objtable_count;i++)draw_objtable_item(i);
   for(int i=0;i<midtable_count;i++)draw_mid(i);
   SDL_Surface*high=pop_hd_capture(onscreen_surface_);if(!high||high->w!=1280)abort();
   SDL_Rect src={640,MAX(0,(Kid.y-65)*4),560,320},dst={(k%5)*280,(k/5)*160,280,160};
   SDL_BlitScaled(high,&src,sheet,&dst);
   fprintf(log,"%d\t%d\t%d\t%d\t%d\n",levelno,frame,direction,objtable_count,midtable_count);count++;
  }
  char p[160];snprintf(p,sizeof(p),"build/guard-engine-level%d-page%d.png",levelno,page);
  if(IMG_SavePNG(sheet,p))abort();
  SDL_FreeSurface(sheet);
 }
 free_peels();fclose(log);
 Guard=saved_guard;Char=saved_char;draw_mode=saved_mode;current_target_surface=saved_target;
 fprintf(stderr,"GUARD exhaustive level=%d poses/directions=%d\n",levelno,count);
}
void SDL_RenderPresent(SDL_Renderer*r){
 void(*present)(SDL_Renderer*)=dlsym(RTLD_NEXT,"SDL_RenderPresent");present(r);SDL_Delay(10);frames++;
 int expected=getenv("POP_GUARD_LEVEL")?atoi(getenv("POP_GUARD_LEVEL")):2;
 if(!done&&frames>5&&current_level==expected&&drawn_room>=1&&Kid.room==drawn_room&&Kid.curr_row<3){
  done=1;audit(expected);SDL_Event e={0};e.type=SDL_QUIT;SDL_PushEvent(&e);
 }
 if(frames>1500&&!done)abort();
}
