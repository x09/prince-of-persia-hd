/* Normalize isolated generated key poses into independent padded frames.
 * GPL-3.0-or-later. No painting or alpha compositing between poses. */
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define COUNT 16
#define SIZE 384
#define WIDTH 576
static Uint32 pixel(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
/* Generated grids can have a few pixels of row drift. Locate real empty
 * gutters instead of slicing through feet at an assumed exact quarter. */
static int gutter(SDL_Surface*s,int horizontal,int target,int radius){
 int best=target;unsigned best_count=~0u;
 for(int k=target-radius;k<=target+radius;k++){
  unsigned count=0;
  for(int a=k-6;a<k+6;a++)for(int b=0;b<(horizontal?s->w:s->h);b++)
   count+=(pixel(s,horizontal?b:a,horizontal?a:b)>>24)>8;
  if(count<best_count || (count==best_count && abs(k-target)<abs(best-target))){best=k;best_count=count;}
 }
 return best;
}
static int check_frames(void){
 for(int i=0;i<COUNT;i++){
  char path[128];snprintf(path,sizeof(path),"data/hd/PERSIALITY/pose%02d.png",i);
  SDL_Surface *raw=IMG_Load(path);if(!raw)return 1;
  SDL_Surface*s=SDL_ConvertSurfaceFormat(raw,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(raw);
  if(!s || s->w!=WIDTH || s->h!=SIZE)return 1;
  unsigned content=0;
  for(int y=0;y<s->h;y++)for(int x=0;x<s->w;x++){
   unsigned a=pixel(s,x,y)>>24;content+=a>32;
   if((x<10||y<10||x>=s->w-10||y>=s->h-10) && a){fprintf(stderr,"Nontransparent margin: %s\n",path);return 1;}
  }
  SDL_FreeSurface(s);if(content<1000)return 1;
 }
 puts("Persiality assets: 16 nonempty independent poses, transparent margins PASS");return 0;
}
int main(int argc,char**argv){
 if(argc==2 && !strcmp(argv[1],"--check"))return check_frames();
 SDL_Surface *raw=IMG_Load("data/hd-source/persiality-clean-16.png");
 if(!raw){fprintf(stderr,"%s\n",IMG_GetError());return 1;}
 SDL_Surface *s=SDL_ConvertSurfaceFormat(raw,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(raw);if(!s)return 1;
 int xs[5]={0,0,0,0,s->w},ys[5]={0,0,0,0,s->h};
 for(int i=1;i<4;i++){xs[i]=gutter(s,0,i*s->w/4,s->w/24);ys[i]=gutter(s,1,i*s->h/4,s->h/24);}
 SDL_Rect boxes[COUNT];double anchors[COUNT],scale=100;
 for(int i=0;i<COUNT;i++){
  int x0=xs[i%4],x1=xs[i%4+1],y0=ys[i/4],y1=ys[i/4+1];
  int l=x1,r=x0,t=y1,b=y0;
  for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++)if((pixel(s,x,y)>>24)>8){
   if(x-x0<6||x1-1-x<6||y-y0<6||y1-1-y<6){fprintf(stderr,"Pose %d touches source cell boundary at %d,%d\n",i,x,y);SDL_FreeSurface(s);return 1;}
   if(x<l)l=x;
   if(x>r)r=x;
   if(y<t)t=y;
   if(y>b)b=y;
  }
  if(r<l||b<t){fprintf(stderr,"Empty pose %d\n",i);SDL_FreeSurface(s);return 1;}
  double sum=0,weight=0;
  for(int y=b-(b-t)/5;y<=b;y++)for(int x=l;x<=r;x++){
   unsigned a=pixel(s,x,y)>>24;if(a>160){sum+=x*a;weight+=a;}
  }
  anchors[i]=weight?sum/weight:(l+r)/2.0;
  boxes[i]=(SDL_Rect){l,t,r-l+1,b-t+1};
  double reach=fmax(anchors[i]-l,r+1-anchors[i]);
  scale=fmin(scale,fmin((WIDTH/2.0-22)/reach,350.0/(b-t+1)));
 }
 SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_NONE);
 for(int i=0;i<COUNT;i++){
  SDL_Surface *out=SDL_CreateRGBSurfaceWithFormat(0,WIDTH,SIZE,32,SDL_PIXELFORMAT_ARGB8888);if(!out){SDL_FreeSurface(s);return 1;}
  SDL_FillRect(out,NULL,0);
  SDL_Rect d={(int)lround(WIDTH/2.0-(anchors[i]-boxes[i].x)*scale),(int)lround(368-boxes[i].h*scale),(int)lround(boxes[i].w*scale),(int)lround(boxes[i].h*scale)};
  if(d.x<12||d.y<12||d.x+d.w>WIDTH-12||d.y+d.h>SIZE-12){fprintf(stderr,"Packing overflow pose %d\n",i);return 1;}
  if(SDL_BlitScaled(s,&boxes[i],out,&d)){fprintf(stderr,"%s\n",SDL_GetError());return 1;}
  char path[256];snprintf(path,sizeof(path),"data/hd/PERSIALITY/pose%02d.png",i);
  if(IMG_SavePNG(out,path)){fprintf(stderr,"%s\n",IMG_GetError());return 1;}
  SDL_FreeSurface(out);
 }
 SDL_FreeSurface(s);printf("Persiality pack: 16 isolated 576x384 frames; shared scale/foot anchor; empty borders PASS\n");return 0;
}
