/* Contact sheets use the engine's frame and sword attachment tables. */
#define POP_HD_INTERNAL
#include "common.h"
#include "hd_weapon.h"
extern const frame_type frame_tbl_guard[];
extern const sword_table_type sword_tbl[];
static SDL_Surface *load(const char *stem,int id,int hd) {
 char p[160];snprintf(p,sizeof(p),"data/%s%s/res%d.png",hd?"hd/":"",stem,id);
 SDL_Surface*s=IMG_Load(p);if(!s)return NULL;
 if(!hd)SDL_SetColorKey(s,SDL_TRUE,0);
 SDL_Surface*t=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return t;
}
static int paint(SDL_Surface*out,const char*stem,int id,int hd,int ox,int bottom,int cx,int cy) {
 SDL_Surface*s=load(stem,id,hd);if(!s)return 1;
 int pad=hd && s->w>200 && strcmp(stem,"PRINCE")?64:0;
 /* Padded body width is detected against the native resource, not guessed. */
 if(hd&&strcmp(stem,"PRINCE")){SDL_Surface*n=load(stem,id,0);if(!n)return 1;pad=(s->w-n->w*4)/2;SDL_FreeSurface(n);}
 SDL_Rect d={cx+ox*4-pad,cy+(bottom+1)*4-(hd?s->h:s->h*4),hd?s->w:s->w*4,hd?s->h:s->h*4};
 SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_BLEND);int rc=SDL_BlitScaled(s,NULL,out,&d);SDL_FreeSurface(s);return rc;
}
int main(void){
 const char*stems[]={"GUARD","FAT","SKEL","VIZIER","SHADOW"};
 FILE*f=fopen("build/guard-pose-audit.tsv","w");if(!f)return 1;
 fprintf(f,"stem\tresource\tframe\tsword_resource\tbody_x\tbody_bottom\tsword_x\tsword_bottom\n");
 for(int g=0;g<5;g++)for(int page=0;page<4;page++){
  SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,1280,960,32,SDL_PIXELFORMAT_ARGB8888);
  SDL_FillRect(out,NULL,0xff202b38);
  for(int k=0;k<8;k++){
   int image=2+page*8+k,idx=-1;for(int i=1;i<40;i++)if(frame_tbl_guard[i].image==image){idx=i;break;}
   if(idx<0)continue;
   char nativepath[160];snprintf(nativepath,sizeof(nativepath),"data/%s/res%d.png",stems[g],751+image);if(access(nativepath,F_OK))continue;
   memset(&Char,0,sizeof(Char));Char.charid=charid_2_guard;Char.frame=149+idx;Char.direction=dir_FF_left;Char.x=120;Char.y=50;
   load_frame_to_obj();
   int bx=calc_screen_x_coord(obj_x),by=obj_y,sw=cur_frame.sword&63;
   int sid=sw?sword_tbl[sw].id:255,sx=bx-(sw?sword_tbl[sw].x:0),sy=by+(sw?sword_tbl[sw].y:0);
   fprintf(f,"%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",stems[g],751+image,Char.frame,sid==255?0:701+sid,bx,by,sx,sy);
   for(int hd=0;hd<2;hd++){
    int cx=(k%2)*640+hd*320+170-bx*4,cy=(k/2)*240+224-(by+1)*4;
    if(paint(out,stems[g],751+image,hd,bx,by,cx,cy))return 2;
    int dx=0,dy=0;if(hd)hd_weapon_grip(g,751+image,-1,&dx,&dy);
    if(sid!=255&&paint(out,"PRINCE",701+sid,hd,sx,sy,cx+dx,cy+dy))return 3;
   }
  }
  char p[160];snprintf(p,sizeof(p),"build/guard-audit-%s-%d.png",stems[g],page);
  if(IMG_SavePNG(out,p))return 4;
  SDL_FreeSurface(out);
 }
 fclose(f);puts("Guard pose audit: 5 types x 32 body resources, engine sword offsets, native/HD pairs");return 0;
}
