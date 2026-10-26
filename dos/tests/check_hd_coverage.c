/* Decode/bind every installed native PNG, not just a handpicked subset. */
#include "../src/hd_game.h"
#include <SDL_image.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){
 const char*stems[]={"KID","GUARD","FAT","SKEL","VIZIER","SHADOW","PRINCE","PV","TITLE","VDUNGEON","VPALACE"};
 int checked=0,failed=0;pop_hd_enable(1);
 for(unsigned g=0;g<sizeof(stems)/sizeof(stems[0]);g++){
  char path[256];snprintf(path,sizeof(path),"data/%s",stems[g]);DIR*d=opendir(path);if(!d)return 2;struct dirent*e;
  while((e=readdir(d))){int id;if(!strstr(e->d_name,".png")||sscanf(e->d_name,"res%d.png",&id)!=1)continue;
   snprintf(path,sizeof(path),"data/%s/res%d.png",stems[g],id);SDL_Surface*n=IMG_Load(path);if(!n){failed++;continue;}SDL_SetColorKey(n,SDL_TRUE,0);
   size_t bytes=(size_t)n->pitch*n->h;void*before=malloc(bytes);memcpy(before,n->pixels,bytes);
   snprintf(path,sizeof(path),"data/hd/%s/res%d.png",stems[g],id);SDL_Surface*a=IMG_Load(path);
   char dat[64];snprintf(dat,sizeof(dat),"%s.DAT",stems[g]);pop_hd_note(dat,id);pop_hd_bind(n);SDL_Surface*h=pop_hd_capture(n);
   if(!a||h==n||h->w!=a->w||h->h!=a->h||a->h!=n->h*4||(a->w!=n->w*4&&a->w!=(n->w+32)*4)||memcmp(before,n->pixels,bytes)){
    fprintf(stderr,"Coverage missing/rejected/mutated: %s %d\n",stems[g],id);failed++;
   }else{
    SDL_Surface*rgba=SDL_ConvertSurfaceFormat(a,SDL_PIXELFORMAT_ARGB8888,0);
    for(int y=0;y<h->h;y++)if(memcmp((Uint8*)h->pixels+y*h->pitch,(Uint8*)rgba->pixels+y*rgba->pitch,h->w*4)){failed++;fprintf(stderr,"Bound pixels differ: %s %d row %d\n",stems[g],id,y);break;}
    SDL_FreeSurface(rgba);
   }free(before);SDL_FreeSurface(a);pop_hd_free(n);checked++;
  }closedir(d);
 }
 pop_hd_shutdown();printf("HD PNG coverage: checked=%d failures=%d\n",checked,failed);return failed?1:0;
}
