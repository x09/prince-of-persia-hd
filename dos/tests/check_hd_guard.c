#include "../src/hd_game.h"
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 int failures=0,count=0;pop_hd_enable(1);
 SDL_Surface*g=SDL_CreateRGBSurfaceWithFormat(0,1600,1000,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_FillRect(g,NULL,0xff293744);
 for(int id=753;id<=784;id++){
  char path[128];snprintf(path,sizeof(path),"data/GUARD/res%d.png",id);
  SDL_Surface*n=IMG_Load(path);if(!n){failures++;continue;}SDL_SetColorKey(n,SDL_TRUE,0);
  size_t size=(size_t)n->pitch*n->h;void*before=malloc(size);memcpy(before,n->pixels,size);
  pop_hd_note("GUARD.DAT",id);pop_hd_bind(n);SDL_Surface*h=pop_hd_capture(n);
  if(h==n||h->w!=(n->w+32)*4||h->h!=n->h*4||memcmp(before,n->pixels,size)){failures++;fprintf(stderr,"guard bind %d failed\n",id);}
  else{
   int opaque=0,transparent=0;
   for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){Uint32 p=px(h,x,y);opaque+=(p>>24)>128;transparent+=!(p>>24);}
   if((id==776?opaque!=0:opaque==0)||!transparent){failures++;fprintf(stderr,"guard alpha %d failed\n",id);}
   SDL_Surface*m=SDL_ConvertSurface(n,n->format,0);pop_hd_mirror(n,m,0);SDL_Surface*f=pop_hd_capture(m);
   if(f->w!=h->w||f->h!=h->h)failures++;
   else for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)if(px(h,x,y)!=px(f,h->w-1-x,y))failures++;
   pop_hd_free(m);
   int k=id-753;SDL_Rect d={(k%8)*200+(200-h->w)/2,(k/8)*250+210-h->h,0,0};
   SDL_SetSurfaceBlendMode(h,SDL_BLENDMODE_BLEND);SDL_BlitSurface(h,NULL,g,&d);
  }
  free(before);pop_hd_free(n);count++;
 }
 FILE*table=fopen("src/seg006.c","r");char line[512];int inside=0,entries=0;
 if(!table)failures++;
 else{while(fgets(line,sizeof(line),table)){
  if(strstr(line,"const frame_type frame_tbl_guard[]")){inside=1;continue;}
  if(!inside)continue;
  if(strstr(line,"};"))break;
  int image;if(sscanf(line," { %d,",&image)==1&&image!=255){
   char path[128];snprintf(path,sizeof(path),"data/hd/GUARD/res%d.png",751+image);
   SDL_Surface*art=IMG_Load(path);if(!art)failures++;SDL_FreeSurface(art);entries++;
  }
 }fclose(table);}
 if(!entries)failures++;
 printf("HD guard frame-table entries=%d\n",entries);
 IMG_SavePNG(g,"build/guard-complete-gallery.png");SDL_FreeSurface(g);
 pop_hd_shutdown();printf("HD guard: resources=%d poses=31 blank=1 mirrored=32 failures=%d\n",count,failures);
 return failures?1:0;
}
