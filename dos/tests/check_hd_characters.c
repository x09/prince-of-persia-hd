#include "../src/hd_game.h"
#include "../tools/hd_characters.h"
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
int main(void){
 int failures=0,total=0,figures=0,blanks=0;pop_hd_enable(1);
 for(unsigned group=0;group<HD_CHARACTER_GROUPS;group++){
  const HDCharacter*c=&hd_characters[group];int rows=(c->count+3)/4;
  SDL_Surface*g=SDL_CreateRGBSurfaceWithFormat(0,1600,rows*300,32,SDL_PIXELFORMAT_ARGB8888);SDL_FillRect(g,NULL,0xff293744);
  for(int k=0;k<c->count;k++){
   int id=c->first+k;char p[256];snprintf(p,sizeof(p),"data/%s/res%d.png",c->stem,id);
   SDL_Surface*n=IMG_Load(p);if(!n){failures++;continue;}
   SDL_SetColorKey(n,SDL_TRUE,0);SDL_Surface*mask=SDL_ConvertSurfaceFormat(n,SDL_PIXELFORMAT_ARGB8888,0);int pixels=0;
   for(int y=0;y<mask->h;y++)for(int x=0;x<mask->w;x++){Uint32 v=px(mask,x,y);pixels+=(v>>24)>0&&(v&0xffffff)!=0;}
   SDL_FreeSurface(mask);
   SDL_SetColorKey(n,SDL_TRUE,0);size_t bytes=(size_t)n->pitch*n->h;void*before=malloc(bytes);memcpy(before,n->pixels,bytes);
   char dat[64];snprintf(dat,sizeof(dat),"%s.DAT",c->stem);pop_hd_note(dat,id);pop_hd_bind(n);SDL_Surface*h=pop_hd_capture(n);
   if(h==n||h->w!=(n->w+32)*4||h->h!=n->h*4||memcmp(before,n->pixels,bytes)){fprintf(stderr,"Rejected/mutated %s %d\n",c->stem,id);failures++;}
   else{
    int opaque=0,transparent=0;for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){Uint32 v=px(h,x,y);opaque+=(v>>24)>128;transparent+=!(v>>24);}
    if((pixels<8?opaque!=0:opaque==0)||!transparent){fprintf(stderr,"Alpha mismatch %s %d\n",c->stem,id);failures++;}
    if(pixels<8)blanks++;else figures++;
    SDL_Surface*m=SDL_ConvertSurface(n,n->format,0);pop_hd_mirror(n,m,0);SDL_Surface*f=pop_hd_capture(m);
    if(f->w!=h->w||f->h!=h->h)failures++;
    else for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++)if(px(h,x,y)!=px(f,h->w-1-x,y))failures++;
    pop_hd_free(m);SDL_Rect d={(k%4)*400+(400-h->w)/2,(k/4)*300+260-h->h,0,0};SDL_SetSurfaceBlendMode(h,SDL_BLENDMODE_BLEND);SDL_BlitSurface(h,NULL,g,&d);
   }
   free(before);pop_hd_free(n);total++;
  }
  char p[256];snprintf(p,sizeof(p),"build/characters-%u-%s.png",group,c->name);IMG_SavePNG(g,p);SDL_FreeSurface(g);
 }
 pop_hd_shutdown();printf("HD characters: resources=%d figures=%d blanks=%d failures=%d\n",total,figures,blanks,failures);return failures?1:0;
}
