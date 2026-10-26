#include "../src/hd_game.h"
#include <SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){
 pop_hd_enable(1);
 for(int id=701;id<=734;id++){
  char p[128];snprintf(p,sizeof(p),"data/PRINCE/res%d.png",id);SDL_Surface*n=IMG_Load(p);assert(n);
  SDL_SetColorKey(n,SDL_TRUE,0);size_t size=(size_t)n->pitch*n->h;
  void*before=malloc(size);memcpy(before,n->pixels,size);
  pop_hd_note("PRINCE.DAT",id);pop_hd_bind(n);SDL_Surface*h=pop_hd_capture(n);
  assert(h!=n&&h->w==n->w*4&&h->h==n->h*4&&!memcmp(before,n->pixels,size));
  int solid=0,edge=0;
  for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){unsigned a=((Uint32*)((Uint8*)h->pixels+y*h->pitch))[x]>>24;solid+=a>128;edge+=a>0&&a<255;}
  assert(solid&&edge);free(before);pop_hd_free(n);
 }
 pop_hd_shutdown();puts("HD swords: 34 bindings, original bounds/native pixels, antialiased coverage: ok");return 0;
}
