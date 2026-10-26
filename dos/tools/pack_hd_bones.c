/* New painted bone pile, continuous across the original two floor pieces.
 * Source extraction/resampling only; game tile bounds stay unchanged. */
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <stdio.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void put(SDL_Surface*s,int x,int y,Uint32 v){((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x]=v;}
static SDL_Surface*load(const char*p){
 SDL_Surface*s=IMG_Load(p);if(!s){fprintf(stderr,"%s: %s\n",p,IMG_GetError());return NULL;}
 SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return r;
}
int main(void){
 SDL_Surface*s=load("data/hd-source/bones.png");if(!s)return 1;
 int l=s->w,t=s->h,r=0,b=0;
 for(int y=0;y<s->h;y++)for(int x=0;x<s->w;x++)if((px(s,x,y)>>24)>=128){
  if(x<l)l=x;
  if(x>r)r=x;
  if(y<t)t=y;
  if(y>b)b=y;
 }
 if(r<=l||b<=t)return 2;
 SDL_Surface*full=SDL_CreateRGBSurfaceWithFormat(0,232,84,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_Surface*art=SDL_CreateRGBSurfaceWithFormat(0,232,84,32,SDL_PIXELFORMAT_ARGB8888);
 SDL_FillRect(full,NULL,0);SDL_FillRect(art,NULL,0);
 for(int k=0;k<2;k++){
  char p[160];snprintf(p,sizeof(p),"data/hd/VDUNGEON/res%d.png",241+k);
  SDL_Surface*floor=load(p);if(!floor)return 3;
  SDL_SetSurfaceBlendMode(floor,SDL_BLENDMODE_NONE);
  SDL_Rect dst={k?128:0,24,0,0};SDL_BlitSurface(floor,NULL,full,&dst);SDL_FreeSurface(floor);
 }
 double angle=-.18,co=cos(angle),si=sin(angle),w=r-l+1,h=b-t+1;
 double scale=fmin(218/(w*co-h*si),76/(h*co-w*si));
 for(int y=0;y<84;y++)for(int x=0;x<232;x++){
  double a=0,red=0,green=0,blue=0;
  for(int yy=0;yy<4;yy++)for(int xx=0;xx<4;xx++){
   double dx=(x+(xx+.5)/4-116)/scale,dy=(y+(yy+.5)/4-40)/scale;
   int sx=(int)floor(l+w/2+co*dx+si*dy),sy=(int)floor(t+h/2-si*dx+co*dy);
   if(sx<0||sx>=s->w||sy<0||sy>=s->h)continue;
   Uint32 v=px(s,sx,sy);double alpha=v>>24;
   a+=alpha;red+=alpha*((v>>16)&255);green+=alpha*((v>>8)&255);blue+=alpha*(v&255);
  }
  if(a)put(art,x,y,((Uint32)(a/16+.5)<<24)|((Uint32)(red/a+.5)<<16)|((Uint32)(green/a+.5)<<8)|(Uint32)(blue/a+.5));
 }
 SDL_SetSurfaceBlendMode(art,SDL_BLENDMODE_BLEND);SDL_BlitSurface(art,NULL,full,NULL);
 IMG_SavePNG(art,"build/bones-art.png");IMG_SavePNG(full,"build/bones-combined.png");
 for(int k=0;k<2;k++){
  SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,k?104:128,k?84:76,32,SDL_PIXELFORMAT_ARGB8888);
  SDL_Rect src={k?128:0,0,out->w,out->h};
  SDL_SetSurfaceBlendMode(full,SDL_BLENDMODE_NONE);SDL_BlitSurface(full,&src,out,NULL);
  char p[160];snprintf(p,sizeof(p),"data/hd/VDUNGEON/res%d.png",230+k);
  if(IMG_SavePNG(out,p))return 4;
  SDL_FreeSurface(out);
 }
 SDL_FreeSurface(s);SDL_FreeSurface(full);SDL_FreeSurface(art);
 puts("HD bones: new painted pile; two continuous pieces 128x76 + 104x84; original floor and native bounds");return 0;
}
