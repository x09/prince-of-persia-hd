/* Parallel 4x composition of DOS blits. Never changes game state or pixels
 * in the native 320x200 simulation/rendering buffers. GPL-3.0-or-later. */
#include "hd_game.h"
#include "config.h"
#include <SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tools/hd_characters.h"
static int character_group(const char*stem,int id){
 for(unsigned i=0;i<HD_CHARACTER_GROUPS;i++)if(!strcmp(stem,hd_characters[i].stem)&&id>=hd_characters[i].first&&id<hd_characters[i].first+hd_characters[i].count)return (int)i;
 return -1;
}
static unsigned character_bound[HD_CHARACTER_GROUPS],character_fallback[HD_CHARACTER_GROUPS];
#define SCALE 4
SDL_Surface*pop_hd_make_glyph(SDL_Surface*,int,const char*);
void pop_hd_font_shutdown(void);
typedef struct Node {
 SDL_Surface *native,*hd;uint64_t palette;int custom,destination,xpad,peel,alpha_layer,scene_fx,hero_art,floor_overlay,blood_art;
 struct Node *next;
} Node;
static Node *nodes;
static SDL_Surface *scene_surface,*scene_background;
static SDL_Surface *floor_surface,*floor_background;
static SDL_Rect scene_effects[32];
static int scene_effect_count;
static int active,visible=1;
static char root[4096],stem[128];
static int resource_id;
static unsigned replacements,blits,presents,failures;
static unsigned hero_bound,hero_fallback,guard_bound,guard_fallback,font_bound,font_fallback;
static SDL_Texture *texture;
static SDL_Renderer *last_renderer;
static int caption=-1;

static uint64_t palette_hash(SDL_Surface *s) {
 uint64_t h=0;if(!s->format->palette)return h;
 SDL_Palette *p=s->format->palette;
 for(int i=0;i<p->ncolors;++i) {
  SDL_Color c=p->colors[i];
  uint32_t rgba=(uint32_t)c.r|((uint32_t)c.g<<8)|((uint32_t)c.b<<16)|((uint32_t)c.a<<24);
  h=(h^rgba)*1099511628211ULL;
 }
 return h;
}
static Node *lookup(SDL_Surface *s) {
 for(Node *n=nodes;n;n=n->next)if(n->native==s)return n;
 return NULL;
}
static SDL_Surface *snapshot(SDL_Surface *s) {
 if(!s || s->w<1 || s->h<1 || s->w>4096 || s->h>4096 ||
    (size_t)s->w*s->h>4000000)return NULL;
 SDL_Surface *rgba=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);
 SDL_Surface *out=SDL_CreateRGBSurfaceWithFormat(0,s->w*SCALE,s->h*SCALE,32,SDL_PIXELFORMAT_ARGB8888);
 if(!rgba || !out){SDL_FreeSurface(rgba);SDL_FreeSurface(out);return NULL;}
 SDL_SetSurfaceBlendMode(rgba,SDL_BLENDMODE_NONE);
 SDL_SetSurfaceColorMod(rgba,255,255,255);SDL_SetSurfaceAlphaMod(rgba,255);
 SDL_SetColorKey(rgba,SDL_FALSE,0);
 if(SDL_BlitScaled(rgba,NULL,out,NULL)){SDL_FreeSurface(out);out=NULL;}
 SDL_FreeSurface(rgba);return out;
}
static Node *get(SDL_Surface *s) {
 if(!active || !s)return NULL;
 Node *n=lookup(s);uint64_t ph=palette_hash(s);
 if(n && !n->custom && !n->destination && n->palette!=ph) {
  SDL_FreeSurface(n->hd);n->hd=NULL;n->palette=ph;
 }
 if(n && !n->hd)n->hd=snapshot(s);
 if(n)return n->hd?n:NULL;
 SDL_Surface *high=snapshot(s);if(!high){++failures;return NULL;}
 n=calloc(1,sizeof(*n));if(!n){SDL_FreeSurface(high);++failures;return NULL;}
 n->native=s;n->hd=high;n->palette=ph;n->next=nodes;nodes=n;return n;
}
static void clip(Node *n) {
 SDL_Rect c;n->destination=1;SDL_GetClipRect(n->native,&c);
 c.x=c.x*SCALE+n->xpad;c.y*=SCALE;c.w*=SCALE;c.h*=SCALE;
 if(n->peel){c.x-=n->xpad;c.w+=2*n->xpad;}
 SDL_SetClipRect(n->hd,&c);
}
static SDL_Rect scaled(SDL_Rect r) {
 r.x*=SCALE;r.y*=SCALE;r.w*=SCALE;r.h*=SCALE;return r;
}
static void source_flags(Node *n) {
 SDL_BlendMode mode;Uint8 r,g,b,a;Uint32 key;
 SDL_GetSurfaceBlendMode(n->native,&mode);
 SDL_GetSurfaceColorMod(n->native,&r,&g,&b);SDL_GetSurfaceAlphaMod(n->native,&a);
 SDL_SetSurfaceColorMod(n->hd,r,g,b);SDL_SetSurfaceAlphaMod(n->hd,a);
 if(SDL_GetColorKey(n->native,&key)==0) {
  Uint8 kr,kg,kb,ka;SDL_GetRGBA(key,n->native->format,&kr,&kg,&kb,&ka);
  SDL_SetColorKey(n->hd,SDL_TRUE,SDL_MapRGBA(n->hd->format,kr,kg,kb,ka));
  SDL_SetSurfaceBlendMode(n->hd,SDL_BLENDMODE_BLEND);
 } else {
  SDL_SetColorKey(n->hd,SDL_FALSE,0);SDL_SetSurfaceBlendMode(n->hd,n->alpha_layer?SDL_BLENDMODE_BLEND:mode);
 }
}
void pop_hd_enable(int enabled) {
 active=enabled!=0;visible=1;
 if(active) {
  char *base=SDL_GetBasePath();snprintf(root,sizeof(root),"%s",base?base:"./");SDL_free(base);
  char probe[8192];snprintf(probe,sizeof(probe),"%sdata/hd/fonts/DejaVuSerif.ttf",root);
  FILE *asset=fopen(probe,"rb");
  if(asset)fclose(asset);
  else {
   base=SDL_GetBasePath();
   snprintf(root,sizeof(root),"%s../share/%s/",base?base:"./",POP_DIR_NAME);SDL_free(base);
   snprintf(probe,sizeof(probe),"%sdata/hd/fonts/DejaVuSerif.ttf",root);
   asset=fopen(probe,"rb");
   if(asset)fclose(asset);else snprintf(root,sizeof(root),"%s/%s/",SHARE_PATH,POP_DIR_NAME);
  }
  fprintf(stderr,"HD: parallel 1280x800 composition enabled; F10 compares DOS/HD\n");
 }
}
int pop_hd_active(void){return active;}
int pop_hd_visible(void){return active && visible;}
void pop_hd_toggle(void){if(active)visible=!visible;}
void pop_hd_note(const char *name,int resource) {
 if(!active)return;
 const char *base=strrchr(name?name:"",'/');base=base?base+1:(name?name:"");
 snprintf(stem,sizeof(stem),"%s",base);char *dot=strrchr(stem,'.');if(dot)*dot=0;
 resource_id=resource;
}
void pop_hd_bind(SDL_Surface *s) {
 if(!active || !s || !stem[0])return;
 char path[8192];snprintf(path,sizeof(path),"%sdata/hd/%s/res%d.png",root,stem,resource_id);
 if(!strcmp(stem,"font")){
  snprintf(path,sizeof(path),"%sdata/hd/fonts/DejaVuSerif.ttf",root);
  SDL_Surface*glyph=pop_hd_make_glyph(s,resource_id-1000,path);
  if(glyph){Node*n=get(s);if(n){SDL_FreeSurface(n->hd);n->hd=glyph;n->custom=1;n->alpha_layer=1;n->xpad=0;++replacements;++font_bound;return;}SDL_FreeSurface(glyph);}
  ++font_fallback;return;
 }
 int hero=!strcmp(stem,"KID") && resource_id>=401 && resource_id<=616;
 int group=character_group(stem,resource_id);
 int guard=!strcmp(stem,"GUARD") && resource_id>=753 && resource_id<=784;
 SDL_RWops *file=SDL_RWFromFile(path,"rb");if(!file){if(hero)++hero_fallback;if(guard)++guard_fallback;if(group>=0)++character_fallback[group];SDL_ClearError();return;}
 SDL_Surface *art=IMG_Load_RW(file,1);
 int padded=hero || guard || group>=0;
 if(!art || (art->w!=s->w*SCALE && !(padded && art->w==(s->w+32)*SCALE)) || art->h!=s->h*SCALE) {
  fprintf(stderr,"HD rejected (size/decode): %s\n",path);++failures;SDL_FreeSurface(art);return;
 }
 SDL_Surface *converted=SDL_ConvertSurfaceFormat(art,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(art);
 if(!converted){++failures;return;}
 Node *n=get(s);if(!n){SDL_FreeSurface(converted);return;}
 SDL_FreeSurface(n->hd);n->hd=converted;n->custom=1;n->xpad=(converted->w-s->w*SCALE)/2;++replacements;
 n->alpha_layer=(!strcmp(stem,"PV")&&resource_id==981)||(!strcmp(stem,"TITLE")&&((resource_id>=42&&resource_id<=45)||(resource_id>=52&&resource_id<=55)));
 n->hero_art=hero;
 n->blood_art=(!strcmp(stem,"VDUNGEON")||!strcmp(stem,"VPALACE"))&&resource_id>=314&&resource_id<=323;
 n->floor_overlay=(!strcmp(stem,"VDUNGEON")||!strcmp(stem,"VPALACE"))&&
  (resource_id==232||resource_id==350||resource_id==351);
 n->scene_fx=(!strcmp(stem,"PRINCE")&&resource_id>=151&&resource_id<=159)||(!strcmp(stem,"PV")&&resource_id>=953&&resource_id<=962);
 if(hero)++hero_bound;
 if(guard)++guard_bound;
 if(group>=0)++character_bound[group];
}
#include "hd_weapon.h"
static int weapon_objects[50][2],weapon_mids[50][2];
static int weapon_pending[2],weapon_draw[2],shadow_pass;
void pop_hd_shadow_draw(SDL_Surface *image,int pass){Node*n=get(image);shadow_pass=n&&n->custom?pass:0;}
void pop_hd_weapon_record(int i,int group,int resource,int direction){
 if(i>=0&&i<50)hd_weapon_grip(group,resource,direction,&weapon_objects[i][0],&weapon_objects[i][1]);
}
void pop_hd_weapon_object(int i){
 weapon_pending[0]=weapon_pending[1]=0;
 if(i>=0&&i<50){weapon_pending[0]=weapon_objects[i][0];weapon_pending[1]=weapon_objects[i][1];}
}
void pop_hd_weapon_mid(int i){
 if(i>=0&&i<50){weapon_mids[i][0]=weapon_pending[0];weapon_mids[i][1]=weapon_pending[1];}
 weapon_pending[0]=weapon_pending[1]=0;
}
void pop_hd_weapon_draw(int i){
 weapon_draw[0]=weapon_draw[1]=0;
 if(i>=0&&i<50){weapon_draw[0]=weapon_mids[i][0];weapon_draw[1]=weapon_mids[i][1];}
}
void pop_hd_weapon_end(void){weapon_draw[0]=weapon_draw[1]=0;shadow_pass=0;}
void pop_hd_weapon_bounds(int i,int *dx,int *dy){
 *dx=*dy=0;
 if(active&&i>=0&&i<50){*dx=weapon_mids[i][0];*dy=weapon_mids[i][1];}
}
int pop_hd_blit(SDL_Surface *s,const SDL_Rect *sr,SDL_Surface *t,SDL_Rect *dr) {
 if(!active||shadow_pass==2)return SDL_UpperBlit(s,sr,t,dr);
 Node *from=get(s),*to=get(t);
 SDL_Rect hs=sr?scaled(*sr):(SDL_Rect){0,0,s?s->w*SCALE:0,s?s->h*SCALE:0};
 SDL_Rect hd=dr?scaled(*dr):(SDL_Rect){0,0,0,0};
 if(from && to) {
  hs.x+=from->xpad;hd.x+=to->xpad;
  int margin=0;
  if(from->peel)margin=from->xpad;
  else if(to->peel)margin=to->xpad;
  else if(from->custom && from->xpad && (!sr || (sr->x==0 && sr->w==s->w)))margin=from->xpad;
  else if(s->w==320 && t->w==320 && s->h>=192 && t->h>=192)margin=16*SCALE;
  if(margin){hs.x-=margin;hs.w+=2*margin;hd.x-=margin;}
 }
 int result=SDL_UpperBlit(s,sr,t,dr);
 if(!result && from && to) {
  clip(to);source_flags(from);
  if(shadow_pass==1&&from->custom){SDL_SetSurfaceAlphaMod(from->hd,190);if(from->hero_art)SDL_SetSurfaceColorMod(from->hd,70,75,100);}
  if(from->custom){hd.x+=weapon_draw[0];hd.y+=weapon_draw[1];}
  if(scene_background&&scene_surface==t&&from->scene_fx){
   if(scene_effect_count<32)scene_effects[scene_effect_count++]=(SDL_Rect){hd.x,hd.y,hs.w,hs.h};
   SDL_SetColorKey(from->hd,SDL_FALSE,0);SDL_SetSurfaceBlendMode(from->hd,SDL_BLENDMODE_BLEND);
  }
  int high_result=0;
  if(from->floor_overlay&&floor_background&&floor_surface==t){
   /* Restore the existing floor, rather than painting a separately textured
    * partial tile (or stretching its last column into a rectangular patch). */
   SDL_Rect bounds;SDL_GetClipRect(to->hd,&bounds);
   for(int y=0;y<hs.h;y++)for(int x=0;x<hs.w+16;x++){
    int dx=hd.x+x,dy=hd.y+y,sx=hs.x+(x<hs.w?x:hs.w-1),sy=hs.y+y;
    if(dx<bounds.x||dy<bounds.y||dx>=bounds.x+bounds.w||dy>=bounds.y+bounds.h||dx<0||dy<0||dx>=floor_background->w||dy>=floor_background->h)continue;
    if(sx<0||sy<0||sx>=from->hd->w||sy>=from->hd->h)continue;
    Uint32 mask=((Uint32*)((Uint8*)from->hd->pixels+sy*from->hd->pitch))[sx];
    unsigned alpha=mask>>24;if(!alpha)continue;
    Uint32 bg=((Uint32*)((Uint8*)floor_background->pixels+dy*floor_background->pitch))[dx];
    if(!(bg&0xffffff))continue; /* The overlay may not create stone in air. */
    Uint32*dst=(Uint32*)((Uint8*)to->hd->pixels+dy*to->hd->pitch)+dx;
    Uint32 value=0xff000000u;
    for(int shift=0;shift<=16;shift+=8)value|=(((bg>>shift&255)*alpha+(*dst>>shift&255)*(255-alpha)+127)/255)<<shift;
    *dst=value;
   }
  }else high_result=SDL_UpperBlit(from->hd,&hs,to->hd,&hd);
  if(high_result) {
   fprintf(stderr,"HD blit error: %dx%d -> %dx%d src=%d,%d,%d,%d dst=%d,%d: %s\n",from->hd->w,from->hd->h,to->hd->w,to->hd->h,hs.x,hs.y,hs.w,hs.h,hd.x,hd.y,SDL_GetError());++failures;
  }
  /* DOS RGB/indexed destinations have no alpha channel. An opaque blit of
   * a custom RGBA tile must clear its transparent backdrop, not copy hidden
   * RGB/alpha into the saved background and later dirty-rectangle copies. */
  int first_y=hd.y<0?0:hd.y,last_y=hd.y+hd.h;
  int first_x=hd.x<0?0:hd.x,last_x=hd.x+hd.w;
  if(last_y>to->hd->h)last_y=to->hd->h;
  if(last_x>to->hd->w)last_x=to->hd->w;
  if(!high_result && !t->format->Amask)for(int y=first_y;y<last_y;++y) {
   Uint32 *row=(Uint32*)((Uint8*)to->hd->pixels+y*to->hd->pitch);
   for(int x=first_x;x<last_x;++x) {
    Uint32 v=row[x];unsigned a=v>>24;
    if(a<255) {
     unsigned r=((v>>16)&255)*a/255,g=((v>>8)&255)*a/255,b=(v&255)*a/255;
     row[x]=0xff000000u|(r<<16)|(g<<8)|b;
    }
   }
  }
  ++blits;
 }
 return result;
}
int pop_hd_fill(SDL_Surface *s,const SDL_Rect *r,Uint32 color) {
 Node *n=get(s);int result=SDL_FillRect(s,r,color);
 if(!result && n) {
  Uint8 cr,cg,cb,ca;SDL_GetRGBA(color,s->format,&cr,&cg,&cb,&ca);
  SDL_Rect h=r?scaled(*r):(SDL_Rect){0,0,n->hd->w,n->hd->h};
  clip(n);SDL_FillRect(n->hd,&h,SDL_MapRGBA(n->hd->format,cr,cg,cb,ca));
 }
 return result;
}
void pop_hd_free(SDL_Surface *s) {
 if(s==scene_surface)pop_hd_scene_base(NULL);
 if(s==floor_surface){SDL_FreeSurface(floor_background);floor_background=NULL;floor_surface=NULL;}
 Node **link=&nodes;
 while(*link) {
  Node *n=*link;if(n->native==s){*link=n->next;SDL_FreeSurface(n->hd);free(n);break;}
  link=&n->next;
 }
 SDL_FreeSurface(s);
}
void pop_hd_expand_peel(SDL_Surface *s) {
 Node *n=get(s);if(!n || n->peel)return;
 SDL_Surface *expanded=SDL_CreateRGBSurfaceWithFormat(0,n->hd->w+128,n->hd->h,32,SDL_PIXELFORMAT_ARGB8888);
 if(!expanded){++failures;return;}
 SDL_FillRect(expanded,NULL,0xff000000u);
 SDL_FreeSurface(n->hd);n->hd=expanded;n->xpad=64;n->peel=n->destination=1;
}
void pop_hd_invalidate(SDL_Surface *s) {
 Node *n=lookup(s);if(!n)return;
 SDL_FreeSurface(n->hd);n->hd=NULL;n->custom=n->destination=n->xpad=n->peel=n->alpha_layer=0;
}
void pop_hd_mirror(SDL_Surface *s,SDL_Surface *t,int vertical) {
 Node *from=get(s),*to=get(t);if(!from || !to)return;
 /* In-place vertical flip uses a full copy to avoid overwriting source. */
 SDL_Surface *copy=SDL_ConvertSurfaceFormat(from->hd,SDL_PIXELFORMAT_ARGB8888,0);
 if(!copy){++failures;return;}
 int w=copy->w,h=copy->h;
 if(to->hd->w!=w || to->hd->h!=h) {
  SDL_Surface *replacement=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
  if(!replacement){SDL_FreeSurface(copy);++failures;return;}
  SDL_FreeSurface(to->hd);to->hd=replacement;
 }
 to->xpad=from->xpad;
 for(int y=0;y<h;++y)for(int x=0;x<w;++x)
  ((Uint32*)((Uint8*)to->hd->pixels+y*to->hd->pitch))[x]=
   ((Uint32*)((Uint8*)copy->pixels+(vertical?h-1-y:y)*copy->pitch))[vertical?x:w-1-x];
 to->custom=from->custom;to->alpha_layer=from->alpha_layer;to->hero_art=from->hero_art;SDL_FreeSurface(copy);
}
void pop_hd_fade(SDL_Surface *t,SDL_Surface *s,int amount) {
 Node *from=get(s),*to=get(t);if(!from || !to)return;
 /* Gameplay fades cover 320x192; the 320x200 target also holds the HUD. */
 if(from->hd->w>to->hd->w || from->hd->h>to->hd->h){fprintf(stderr,"HD fade error: %dx%d -> %dx%d\n",from->hd->w,from->hd->h,to->hd->w,to->hd->h);++failures;return;}
 for(int y=0;y<from->hd->h;++y)for(int x=0;x<from->hd->w;++x) {
  Uint32 p=((Uint32*)((Uint8*)from->hd->pixels+y*from->hd->pitch))[x];
  int r=(p>>16)&255,g=(p>>8)&255,b=p&255;
  r=r>amount?r-amount:0;g=g>amount?g-amount:0;b=b>amount?b-amount:0;
  ((Uint32*)((Uint8*)to->hd->pixels+y*to->hd->pitch))[x]=0xff000000u|((unsigned)r<<16)|((unsigned)g<<8)|(unsigned)b;
 }
 to->destination=1;
}
SDL_Surface *pop_hd_capture(SDL_Surface *s) {
 if(!active || !visible)return s;
 Node *n=get(s);return n?n->hd:s;
}
int pop_hd_present(SDL_Renderer *r,SDL_Surface *s) {
 if(!active)return 0;
 if(caption!=visible) {
  SDL_Window *w=SDL_RenderGetWindow(r);if(w)SDL_SetWindowTitle(w,visible?"PoPHD - F10: DOS":"PoPHD (DOS graphics) - F10: HD");
  caption=visible;
 }
 if(!visible)return 0;
 Node *n=get(s);if(!n)return 0;
 if(r!=last_renderer) {
  if(texture)SDL_DestroyTexture(texture);
  texture=NULL;last_renderer=r;
 }
 if(!texture) {
  texture=SDL_CreateTexture(r,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,n->hd->w,n->hd->h);
  if(!texture){++failures;return 0;}
  SDL_SetTextureBlendMode(texture,SDL_BLENDMODE_NONE);
  SDL_SetTextureScaleMode(texture,SDL_ScaleModeNearest);
 }
 if(SDL_UpdateTexture(texture,NULL,n->hd->pixels,n->hd->pitch)){++failures;return 0;}
 SDL_SetRenderTarget(r,NULL);SDL_SetRenderDrawColor(r,0,0,0,255);SDL_RenderClear(r);SDL_RenderCopy(r,texture,NULL,NULL);++presents;
 return 1;
}
void pop_hd_scene_base(SDL_Surface*s){
 SDL_FreeSurface(scene_background);scene_background=NULL;scene_surface=NULL;scene_effect_count=0;
 if(!active||!s)return;
 Node*n=get(s);if(!n)return;
 scene_background=SDL_ConvertSurfaceFormat(n->hd,SDL_PIXELFORMAT_ARGB8888,0);
 if(scene_background){scene_surface=s;SDL_SetSurfaceBlendMode(scene_background,SDL_BLENDMODE_NONE);}
}
void pop_hd_scene_prepare(SDL_Surface*s){
 if(!active||!scene_background||scene_surface!=s)return;
 Node*n=get(s);if(!n)return;clip(n);
 for(int i=0;i<scene_effect_count;i++){SDL_Rect src=scene_effects[i],dst=src;SDL_BlitSurface(scene_background,&src,n->hd,&dst);}
 scene_effect_count=0;
}
void pop_hd_floor_background(SDL_Surface*surface){
 if(!active)return;
 Node*n=get(surface);if(!n)return;
 if(floor_background&&(floor_background->w!=n->hd->w||floor_background->h!=n->hd->h)){SDL_FreeSurface(floor_background);floor_background=NULL;}
 if(!floor_background)floor_background=SDL_CreateRGBSurfaceWithFormat(0,n->hd->w,n->hd->h,32,SDL_PIXELFORMAT_ARGB8888);
 if(!floor_background){++failures;return;}
 SDL_SetClipRect(floor_background,NULL);SDL_SetSurfaceBlendMode(n->hd,SDL_BLENDMODE_NONE);
 if(SDL_BlitSurface(n->hd,NULL,floor_background,NULL))++failures;
 floor_surface=surface;
}
void pop_hd_outline(SDL_Surface*target,const SDL_Rect*rect,Uint32 color){
 if(!active||!rect||rect->w<1||rect->h<1)return;
 Node*n=get(target);if(!n)return;clip(n);Uint8 r,g,b,a;SDL_GetRGBA(color,target->format,&r,&g,&b,&a);
 Uint32 v=SDL_MapRGBA(n->hd->format,r,g,b,255);SDL_Rect q=scaled(*rect);q.x+=n->xpad;
 SDL_Rect sides[4]={{q.x,q.y,q.w,4},{q.x,q.y+q.h-4,q.w,4},{q.x,q.y,4,q.h},{q.x+q.w-4,q.y,4,q.h}};
 for(int i=0;i<4;i++)SDL_FillRect(n->hd,&sides[i],v);
}
void pop_hd_xor(SDL_Surface*target,SDL_Surface*source,const SDL_Rect*rect){
 if(!active||!rect||shadow_pass==2)return;
 Node*to=get(target),*from=get(source);if(!to||!from)return;
 for(int y=0;y<to->hd->h;y++)for(int x=0;x<to->hd->w;x++){
  int sx=rect->x*4+x+from->xpad,sy=rect->y*4+y;if(sx<0||sy<0||sx>=from->hd->w||sy>=from->hd->h)continue;
  Uint32 s=((Uint32*)((Uint8*)from->hd->pixels+sy*from->hd->pitch))[sx];
  if((s>>24)<128)continue;
  Uint32*d=(Uint32*)((Uint8*)to->hd->pixels+y*to->hd->pitch)+x;*d=0xff000000|((*d^s)&0xffffff);
 }
}
void pop_hd_mono(SDL_Surface*source,SDL_Surface*target,Uint32 rgb){
 if(!active)return;
 Node*from=get(source),*to=get(target);if(!from||!to)return;
 SDL_Surface*copy=SDL_ConvertSurfaceFormat(from->hd,SDL_PIXELFORMAT_ARGB8888,0);if(!copy){++failures;return;}
 /* These masks are monochrome only in the DOS renderer. Keep the painted
  * blood's wet highlights and dark red variations in the HD companion. */
 for(int y=0;!from->blood_art&&y<copy->h;y++)for(int x=0;x<copy->w;x++){
  Uint32*p=(Uint32*)((Uint8*)copy->pixels+y*copy->pitch)+x;*p=(*p&0xff000000)|(rgb&0xffffff);
 }
 SDL_FreeSurface(to->hd);to->hd=copy;to->custom=from->custom;to->alpha_layer=from->alpha_layer;to->xpad=from->xpad;
}
void pop_hd_shutdown(void) {
 SDL_FreeSurface(floor_background);floor_background=NULL;floor_surface=NULL;
 pop_hd_scene_base(NULL);
 pop_hd_font_shutdown();
 if(active) {
  fprintf(stderr,"HD: art bindings=%u blits=%u presents=%u failures=%u\n",replacements,blits,presents,failures);
  fprintf(stderr,"HD animations: bound=%u fallback=%u (KID 401..616, includes 3 mouse frames)\n",hero_bound,hero_fallback);
  fprintf(stderr,"HD guards: bound=%u fallback=%u (GUARD 753..784)\n",guard_bound,guard_fallback);
  fprintf(stderr,"HD font: bound=%u fallback=%u\n",font_bound,font_fallback);
  for(unsigned i=0;i<HD_CHARACTER_GROUPS;i++)fprintf(stderr,"HD character %s %d..%d: bound=%u fallback=%u\n",hd_characters[i].name,hd_characters[i].first,hd_characters[i].first+hd_characters[i].count-1,character_bound[i],character_fallback[i]);
 }
 active=0;
 if(texture)SDL_DestroyTexture(texture);
 texture=NULL;last_renderer=NULL;
 while(nodes){Node *next=nodes->next;SDL_FreeSurface(nodes->hd);free(nodes);nodes=next;}
}
