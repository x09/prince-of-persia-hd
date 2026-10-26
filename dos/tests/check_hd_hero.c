#include "../src/hd_game.h"
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned pixel(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 int failures=0,checked=0;pop_hd_enable(1);
 SDL_Surface*gallery=SDL_CreateRGBSurfaceWithFormat(0,1280,960,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_FillRect(gallery,NULL,0xff293744);
 const int pick[16]={401,407,416,423,424,445,454,462,491,502,519,537,553,578,603,616};
 for(int id=401;id<=616;id++){
  char path[128];snprintf(path,sizeof(path),"data/KID/res%d.png",id);SDL_Surface*n=IMG_Load(path);if(!n){failures++;continue;}
  SDL_SetColorKey(n,SDL_TRUE,0);
  size_t bytes=(size_t)n->pitch*n->h;void*before=malloc(bytes);memcpy(before,n->pixels,bytes);
  pop_hd_note("KID.DAT",id);pop_hd_bind(n);SDL_Surface*h=pop_hd_capture(n);
  snprintf(path,sizeof(path),"data/hd/KID/res%d.png",id);SDL_Surface*art=IMG_Load(path);
  if(!art || h==n || h->w!=art->w || art->w!=(n->w+32)*4 || h->h!=n->h*4 || memcmp(before,n->pixels,bytes)){
   fprintf(stderr,"HD hero %d: missing, rejected or native pixels changed\n",id);failures++;
  }else{
   unsigned seen[4096]={0},colours=0,opaque=0,transparent=0;
   for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){
    unsigned v=pixel(h,x,y);if((v>>24)>128){opaque++;unsigned q=((v>>12)&0xf00)|((v>>8)&0xf0)|((v>>4)&15);if(!seen[q]++){colours++;}}
    if(!(v>>24))transparent++;
   }
   if(!opaque||!transparent||colours<16){fprintf(stderr,"HD hero %d: empty/opaque/low-colour art\n",id);failures++;}
   for(int k=0;k<16;k++)if(id==pick[k]){
    SDL_SetSurfaceBlendMode(h,SDL_BLENDMODE_BLEND);
    SDL_Rect dst={(k%8)*160+(160-h->w)/2,(k/8)*480+300-h->h,h->w,h->h};
    SDL_BlitSurface(h,NULL,gallery,&dst);
   }
  }
  SDL_FreeSurface(art);free(before);pop_hd_free(n);checked++;
 }
 /* Audit every valid frame table entry rather than assuming contiguous phases. */
 FILE*f=fopen("src/seg006.c","r");char line[512];int inside=0,entries=0;
 if(!f)failures++;
 else {while(fgets(line,sizeof(line),f)){
  if(strstr(line,"const frame_type frame_table_kid[]")){inside=1;continue;}
  if(!inside)continue;
  if(strstr(line,"};"))break;
  int image;if(sscanf(line," { %d,",&image)==1 && image!=255){
   char path[128];snprintf(path,sizeof(path),"data/hd/KID/res%d.png",401+image);
   SDL_Surface*s=IMG_Load(path);if(!s){fprintf(stderr,"frame image %d missing\n",image);failures++;}SDL_FreeSurface(s);entries++;
  }
 }fclose(f);}
 IMG_SavePNG(gallery,"build/hero-complete-gallery.png");SDL_FreeSurface(gallery);pop_hd_shutdown();
 printf("HD hero: checked=%d hero=213 mouse=3 frame-table-entries=%d failures=%d\n",checked,entries,failures);
 return failures?1:0;
}
