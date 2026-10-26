/* Regression: HD weapon translation, mirrored poses, native pixel identity,
 * no offset leaking into the following draw, and complete background restore. */
#define POP_HD_INTERNAL
#include "common.h"
#include <assert.h>
extern const frame_type frame_tbl_guard[];
SDL_Surface *hflip(SDL_Surface *input);
void blit_xor(SDL_Surface *,SDL_Rect *,SDL_Surface *,SDL_Rect *);
extern const sword_table_type sword_tbl[];
static Uint32 pixel(SDL_Surface*s,int x,int y){return ((Uint32*)((byte*)s->pixels+y*s->pitch))[x];}
int main(void){
 pop_hd_enable(1);int checked=0;
 SDL_Surface*t=SDL_CreateRGBSurfaceWithFormat(0,320,200,32,SDL_PIXELFORMAT_ARGB8888);
 assert(t);Uint32 black=0xff000000;
 for(int group=0;group<5;group++)for(int id=753;id<=775;id++){
  int fi=-1;for(int i=1;i<40;i++)if(frame_tbl_guard[i].image==id-751){fi=i;break;}
  if(fi<0)continue;
  int sw=frame_tbl_guard[fi].sword&63;
  if(!sw||sword_tbl[sw].id==255)continue;
  char path[160];snprintf(path,sizeof(path),"data/PRINCE/res%d.png",701+sword_tbl[sw].id);
  SDL_Surface*s=IMG_Load(path);assert(s);SDL_SetColorKey(s,SDL_TRUE,0);
  pop_hd_note("PRINCE.DAT",701+sword_tbl[sw].id);pop_hd_bind(s);
  for(int direction=-1;direction<=0;direction++){
   SDL_Surface*image=s;if(direction==0)image=hflip(s);
   pop_hd_fill(t,NULL,black);SDL_Rect at={100,80,0,0};pop_hd_blit(image,NULL,t,&at);
   SDL_Surface*base=SDL_ConvertSurface(pop_hd_capture(t),pop_hd_capture(t)->format,0);assert(base);
   size_t size=t->pitch*t->h;void*native=malloc(size);assert(native);memcpy(native,t->pixels,size);
   pop_hd_fill(t,NULL,black);
   pop_hd_weapon_record(0,group,id,direction);pop_hd_weapon_object(0);pop_hd_weapon_mid(0);
   int dx,dy;pop_hd_weapon_bounds(0,&dx,&dy);
   pop_hd_weapon_draw(0);at=(SDL_Rect){100,80,0,0};pop_hd_blit(image,NULL,t,&at);pop_hd_weapon_end();
   assert(!memcmp(native,t->pixels,size));
   SDL_Surface*shifted=pop_hd_capture(t);
   for(int y=0;y<shifted->h;y++)for(int x=0;x<shifted->w;x++){
    int xx=x-dx,yy=y-dy;
    Uint32 expected=xx>=0&&yy>=0&&xx<base->w&&yy<base->h?pixel(base,xx,yy):black;
    assert(pixel(shifted,x,y)==expected);
   }
   /* Restoring the union of old/new rectangles must leave no weapon trail. */
   int mx=(abs(dx)+3)/4,my=(abs(dy)+3)/4;
   SDL_Rect erase={100-mx,80-my,image->w+2*mx,image->h+2*my};
   pop_hd_fill(t,&erase,black);
   for(int y=0;y<shifted->h;y++)for(int x=0;x<shifted->w;x++)assert(pixel(shifted,x,y)==black);
   at=(SDL_Rect){100,80,0,0};pop_hd_blit(image,NULL,t,&at);
   shifted=pop_hd_capture(t);
   for(int y=0;y<shifted->h;y++)for(int x=0;x<shifted->w;x++)assert(pixel(shifted,x,y)==pixel(base,x,y));
   free(native);SDL_FreeSurface(base);if(image!=s)pop_hd_free(image);checked++;
  }
  pop_hd_free(s);
 }
 SDL_Surface*shadow=IMG_Load("data/SHADOW/res753.png");assert(shadow);
 SDL_SetColorKey(shadow,SDL_TRUE,0);pop_hd_note("SHADOW.DAT",753);pop_hd_bind(shadow);
 pop_hd_fill(t,NULL,black);SDL_Rect sd={100,80,shadow->w,shadow->h};
 pop_hd_shadow_draw(shadow,1);pop_hd_blit(shadow,NULL,t,&sd);pop_hd_weapon_end();
 SDL_Surface*before=SDL_ConvertSurface(pop_hd_capture(t),pop_hd_capture(t)->format,0);assert(before);
 size_t ns=t->pitch*t->h;void*nb=malloc(ns);memcpy(nb,t->pixels,ns);
 sd=(SDL_Rect){100,80,shadow->w,shadow->h};SDL_Rect ss={0,0,shadow->w,shadow->h};
 pop_hd_shadow_draw(shadow,2);blit_xor(t,&sd,shadow,&ss);pop_hd_weapon_end();
 assert(memcmp(nb,t->pixels,ns));SDL_Surface*after=pop_hd_capture(t);
 for(int y=0;y<after->h;y++)for(int x=0;x<after->w;x++)assert(pixel(before,x,y)==pixel(after,x,y));
 free(nb);SDL_FreeSurface(before);pop_hd_free(shadow);
 SDL_Surface*kid=IMG_Load("data/KID/res401.png");assert(kid);
 SDL_SetColorKey(kid,SDL_TRUE,0);pop_hd_note("KID.DAT",401);pop_hd_bind(kid);
 for(int direction=0;direction<2;direction++){
  SDL_Surface*s=direction?hflip(kid):kid;
  pop_hd_fill(t,NULL,black);SDL_Rect d={100,80,0,0};pop_hd_blit(s,NULL,t,&d);
  SDL_Surface*normal=SDL_ConvertSurface(pop_hd_capture(t),pop_hd_capture(t)->format,0);assert(normal);
  pop_hd_fill(t,NULL,black);pop_hd_shadow_draw(s,1);d=(SDL_Rect){100,80,0,0};pop_hd_blit(s,NULL,t,&d);pop_hd_weapon_end();
  SDL_Surface*dark=pop_hd_capture(t);int different=0;
  for(int y=0;y<dark->h;y++)for(int x=0;x<dark->w;x++)different+=pixel(dark,x,y)!=pixel(normal,x,y);
  assert(different>100);
  pop_hd_fill(t,NULL,black);d=(SDL_Rect){100,80,0,0};pop_hd_blit(s,NULL,t,&d);dark=pop_hd_capture(t);
  for(int y=0;y<dark->h;y++)for(int x=0;x<dark->w;x++)assert(pixel(dark,x,y)==pixel(normal,x,y));
  SDL_FreeSurface(normal);if(s!=kid)pop_hd_free(s);
 }
 pop_hd_free(kid);
 puts("HD shadow: translucent painting, dark shared fall poses, native XOR and normal hero colours: ok");
 pop_hd_free(t);pop_hd_shutdown();
 printf("HD grips: %d poses/directions, exact HD translation, native identity, restore and scope isolation: ok\n",checked);
 return checked==220?0:1;
}
