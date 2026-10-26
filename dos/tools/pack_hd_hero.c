/* Complete HD animation packing. Generated figures retain DOS resource bounds.
 * Source atlas extraction and alpha-aware area resampling only; no pose substitution. */
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hd_fit.h"
typedef struct {int x0,y0,x1,y1,area,cell;} Part;
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void put(SDL_Surface*s,int x,int y,Uint32 p){((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x]=p;}
static SDL_Surface*load(const char*p){SDL_Surface*s=IMG_Load(p);if(!s){fprintf(stderr,"%s: %s\n",p,IMG_GetError());return NULL;}SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return r;}
static int sheet(int group,const int ids[16],FILE*report){
 char path[256];if(group==12)snprintf(path,sizeof(path),"data/hd-source/hero-climb-corrected.png");
 else snprintf(path,sizeof(path),"data/hd-source/hero-full-%02d.png",group);
 SDL_Surface*s=load(path);if(!s)return 1;
 int w=s->w,h=s->h,n=w*h,*labels=calloc((size_t)n,sizeof(int)),*q=malloc((size_t)n*sizeof(int));
 Part*parts=calloc((size_t)n/2+1,sizeof(Part));int count=0,best[16]={0};
 if(!labels||!q||!parts)return 1;
 for(int seed=0;seed<n;seed++){
  if(labels[seed]||(px(s,seed%w,seed/w)>>24)<192)continue;
  int id=++count;Part*a=&parts[id];*a=(Part){w,h,0,0,0,0};int head=0,tail=0;q[tail++]=seed;labels[seed]=id;
  while(head<tail){
   int v=q[head++],x=v%w,y=v/w;if(x<a->x0)a->x0=x;if(x>a->x1)a->x1=x;if(y<a->y0)a->y0=y;if(y>a->y1)a->y1=y;++a->area;
   int near[4]={x?v-1:-1,x+1<w?v+1:-1,y?v-w:-1,y+1<h?v+w:-1};
   for(int k=0;k<4;k++)if(near[k]>=0&&!labels[near[k]]&&(px(s,near[k]%w,near[k]/w)>>24)>=192){labels[near[k]]=id;q[tail++]=near[k];}
  }
  int col=((a->x0+a->x1)*2)/w,row=((a->y0+a->y1)*2)/h;if(col>3)col=3;if(row>3)row=3;
  a->cell=row*4+col;if(a->area>parts[best[a->cell]].area)best[a->cell]=id;
 }
 for(int cell=0;cell<16;cell++){
  if(!ids[cell])continue;
  int id=best[cell];Part a=parts[id];if(!id||a.area<100){fprintf(stderr,"sheet %d cell %d: missing figure\n",group,cell);return 1;}
  /* Keep sizeable disconnected hands/tails assigned to the same grid cell. */
  int minarea=a.area/100;if(minarea<48)minarea=48;
  for(int k=1;k<=count;k++)if(parts[k].cell==cell&&parts[k].area>=minarea){
   if(parts[k].x0<a.x0)a.x0=parts[k].x0;
   if(parts[k].y0<a.y0)a.y0=parts[k].y0;
   if(parts[k].x1>a.x1)a.x1=parts[k].x1;
   if(parts[k].y1>a.y1)a.y1=parts[k].y1;
  }
  SDL_Surface*cut=SDL_CreateRGBSurfaceWithFormat(0,a.x1-a.x0+1,a.y1-a.y0+1,32,SDL_PIXELFORMAT_ARGB8888);
  SDL_FillRect(cut,NULL,0);
  for(int y=a.y0;y<=a.y1;y++)for(int x=a.x0;x<=a.x1;x++){
   int k=labels[y*w+x];if(k&&parts[k].cell==cell&&parts[k].area>=minarea)put(cut,x-a.x0,y-a.y0,px(s,x,y)|0xff000000u);
  }
  snprintf(path,sizeof(path),"data/KID/res%d.png",ids[cell]);SDL_Surface*native=IMG_Load(path);if(!native)return 1;
  SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,(native->w+32)*4,native->h*4,32,SDL_PIXELFORMAT_ARGB8888);if(!out)return 1;
  SDL_FillRect(out,NULL,0);if(hd_fit(cut,out)){fprintf(stderr,"pose %d exceeds padded canvas\n",ids[cell]);return 1;}
  snprintf(path,sizeof(path),"data/hd/KID/res%d.png",ids[cell]);if(IMG_SavePNG(out,path))return 1;
  fprintf(report,"%d sheet=%02d cell=%d source=%dx%d target=%dx%d area=%d\n",ids[cell],group,cell,cut->w,cut->h,out->w,out->h,parts[id].area);
  SDL_FreeSurface(native);SDL_FreeSurface(cut);SDL_FreeSurface(out);
 }
 free(labels);free(q);free(parts);SDL_FreeSurface(s);return 0;
}
int main(int argc,char**argv){
 int map[12][16]={{0}},g,c,id,valid;FILE*f=fopen("data/hd-source/hero-map.txt","r");if(!f)return 1;
 while(fscanf(f,"%d %d %d %d",&g,&c,&id,&valid)==4) {
  if(valid&&g>=0&&g<12&&c>=0&&c<16)map[g][c]=id;
 }
 fclose(f);
 FILE*r=fopen(argc>1&&!strcmp(argv[1],"--climb-only")?"data/hd/hero-climb-manifest.txt":"data/hd/hero-full-manifest.txt","w");if(!r)return 1;
 int climb_only=argc>1&&!strcmp(argv[1],"--climb-only");
 int limit=climb_only?0:(argc>1?atoi(argv[1]):12);if(!climb_only&&(limit<1||limit>12))return 1;
 for(g=0;g<limit;g++)if(sheet(g,map[g],r)){fclose(r);return 1;}
 int climb[16];for(int i=0;i<16;i++)climb[i]=546+i;
 if(sheet(12,climb,r)){fclose(r);return 1;}
 fclose(r);printf("HD animation sheets packed=%d plus climbing correction (no native upscaling or pose reuse)\n",limit);return 0;
}