/* Complete HD animation packing. Generated figures retain DOS resource bounds.
 * Source atlas extraction and alpha-aware area resampling only; no pose substitution. */
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hd_fit.h"
#include "hd_characters.h"
typedef struct {int x0,y0,x1,y1,area,cell;} Part;
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void put(SDL_Surface*s,int x,int y,Uint32 p){((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x]=p;}
static SDL_Surface*load(const char*p){SDL_Surface*s=IMG_Load(p);if(!s){fprintf(stderr,"%s: %s\n",p,IMG_GetError());return NULL;}SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return r;}
static int sheet(const HDCharacter*g,FILE*report){
 char path[256];snprintf(path,sizeof(path),"data/hd-source/%s.png",g->name);
 SDL_Surface*s=load(path);if(!s)return 1;
 int cols=g->cols,rows=g->rows;
 int w=s->w,h=s->h,n=w*h,*labels=calloc((size_t)n,sizeof(int)),*q=malloc((size_t)n*sizeof(int));
 Part*parts=calloc((size_t)n/2+1,sizeof(Part));int count=0,best[36]={0};
 if(!labels||!q||!parts)return 1;
 for(int seed=0;seed<n;seed++){
  if(labels[seed]||(px(s,seed%w,seed/w)>>24)<192)continue;
  int id=++count;Part*a=&parts[id];*a=(Part){w,h,0,0,0,0};int head=0,tail=0;q[tail++]=seed;labels[seed]=id;
  while(head<tail){
   int v=q[head++],x=v%w,y=v/w;if(x<a->x0)a->x0=x;if(x>a->x1)a->x1=x;if(y<a->y0)a->y0=y;if(y>a->y1)a->y1=y;++a->area;
   int near[4]={x?v-1:-1,x+1<w?v+1:-1,y?v-w:-1,y+1<h?v+w:-1};
   for(int k=0;k<4;k++)if(near[k]>=0&&!labels[near[k]]&&(px(s,near[k]%w,near[k]/w)>>24)>=192){labels[near[k]]=id;q[tail++]=near[k];}
  }
  int col=((a->x0+a->x1)*cols)/(2*w),row=((a->y0+a->y1)*rows)/(2*h);if(col>=cols)col=cols-1;if(row>=rows)row=rows-1;
  a->cell=row*cols+col;if(a->area>parts[best[a->cell]].area)best[a->cell]=id;
 }
 for(int cell=0;cell<g->count;cell++){
  int resource=g->first+cell;
  snprintf(path,sizeof(path),"data/%s/res%d.png",g->stem,resource);
  SDL_Surface*native=IMG_Load(path);if(!native)return 1;
  SDL_SetColorKey(native,SDL_TRUE,0);
  SDL_Surface*mask=SDL_ConvertSurfaceFormat(native,SDL_PIXELFORMAT_ARGB8888,0);if(!mask)return 1;
  int pixels=0;for(int y=0;y<mask->h;y++)for(int x=0;x<mask->w;x++){
   Uint32 v=px(mask,x,y);pixels+=(v>>24)>0&&(v&0xffffff)!=0;
  }SDL_FreeSurface(mask);
  if(pixels<8){
   SDL_Surface*empty=SDL_CreateRGBSurfaceWithFormat(0,(native->w+32)*4,native->h*4,32,SDL_PIXELFORMAT_ARGB8888);
   SDL_FillRect(empty,NULL,0);snprintf(path,sizeof(path),"data/hd/%s/res%d.png",g->stem,resource);
   if(IMG_SavePNG(empty,path))return 1;
   fprintf(report,"%s %d blank native-pixels=%d\n",g->stem,resource,pixels);
   SDL_FreeSurface(empty);SDL_FreeSurface(native);continue;
  }
  int id=best[cell];Part a=parts[id];if(!id||a.area<100){fprintf(stderr,"sheet %s cell %d: missing figure\n",g->name,cell);return 1;}
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
  SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,(native->w+32)*4,native->h*4,32,SDL_PIXELFORMAT_ARGB8888);if(!out)return 1;
  SDL_FillRect(out,NULL,0);if(hd_fit(cut,out)){fprintf(stderr,"pose %d exceeds padded canvas\n",resource);return 1;}
  snprintf(path,sizeof(path),"data/hd/%s/res%d.png",g->stem,resource);if(IMG_SavePNG(out,path))return 1;
  fprintf(report,"%s %d sheet=%s cell=%d source=%dx%d target=%dx%d area=%d\n",g->stem,resource,g->name,cell,cut->w,cut->h,out->w,out->h,parts[id].area);
  SDL_FreeSurface(native);SDL_FreeSurface(cut);SDL_FreeSurface(out);
 }
 free(labels);free(q);free(parts);SDL_FreeSurface(s);return 0;
}
int main(void){
 FILE*r=fopen("data/hd/characters-manifest.txt","w");if(!r)return 1;
 for(unsigned i=0;i<HD_CHARACTER_GROUPS;i++){if(sheet(&hd_characters[i],r))return 1;}
 fclose(r);puts("HD characters: 205 resources packed, native blank frames preserved");return 0;
}
