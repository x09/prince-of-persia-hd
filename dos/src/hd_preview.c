/* Experimental HD art evaluation mode; game simulation remains untouched. */
#include "hd_preview.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CELL_W 256
#define CELL_H 208
static char root[4096];
static void find_root(void) {
 char *base=SDL_GetBasePath();snprintf(root,sizeof(root),"%s",base?base:"./");SDL_free(base);
}
static SDL_Surface *read_surface(const char *name) {
 char path[8192];snprintf(path,sizeof(path),"%s%s",root,name);
 SDL_Surface *s=IMG_Load(path);
 if(!s)fprintf(stderr,"HD asset %s: %s\n",path,IMG_GetError());
 return s;
}
int pop_hd_assets_check(void) {
 find_root();
 SDL_Surface *room=read_surface("data/hd-preview/room.png");
 SDL_Surface *source=read_surface("data/hd-preview/run.png");
 if(!room || !source){SDL_FreeSurface(room);SDL_FreeSurface(source);return 1;}
 if(room->w<1280 || room->h<768 || source->w!=4*CELL_W || source->h!=2*CELL_H) {
  fputs("HD assets: incorrect dimensions\n",stderr);SDL_FreeSurface(room);SDL_FreeSurface(source);return 1;
 }
 SDL_Surface *s=SDL_ConvertSurfaceFormat(source,SDL_PIXELFORMAT_RGBA32,0);
 SDL_FreeSurface(source);if(!s){SDL_FreeSurface(room);return 1;}
 uint64_t hashes[8];unsigned failed=0;
 for(int i=0;i<8;++i) {
  unsigned visible=0,clear=0;int ymin=CELL_H,ymax=-1;
  uint64_t hash=1469598103934665603ULL;
  for(int y=0;y<CELL_H;++y)for(int x=0;x<CELL_W;++x) {
   Uint32 pixel=((Uint32*)((Uint8*)s->pixels+(i/4*CELL_H+y)*s->pitch))[i%4*CELL_W+x];
   Uint8 r,g,b,a;SDL_GetRGBA(pixel,s->format,&r,&g,&b,&a);
   hash=(hash^pixel)*1099511628211ULL;
   if(a>127){++visible;if(y<ymin)ymin=y;if(y>ymax)ymax=y;}else if(!a)++clear;
   if((x==0 || x==CELL_W-1 || y==0 || y==CELL_H-1) && a)++failed;
  }
  if(visible<3000 || clear<20000 || ymax-ymin<160 || ymax>194)++failed;
  hashes[i]=hash;for(int j=0;j<i;++j)if(hashes[j]==hash)++failed;
  printf("HD pose %d: visible=%u clear=%u y=%d..%d hash=%llx\n",i+7,visible,clear,ymin,ymax,(unsigned long long)hash);
 }
 printf("HD assets: %dx%d room, 8 distinct transparent registered poses, failures=%u\n",room->w,room->h,failed);
 SDL_FreeSurface(room);SDL_FreeSurface(s);return failed!=0;
}
static int save_view(SDL_Renderer *r,const char *path) {
 int w=0,h=0;SDL_GetRendererOutputSize(r,&w,&h);
 SDL_Surface *s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
 int result=!s || SDL_RenderReadPixels(r,NULL,SDL_PIXELFORMAT_ARGB8888,s->pixels,s->pitch) || IMG_SavePNG(s,path);
 if(s)SDL_FreeSurface(s);
 if(result)fprintf(stderr,"HD capture: %s\n",SDL_GetError());
 return result!=0;
}
int pop_hd_preview(int argc,char **argv) {
 unsigned smoke=0;const char *capture=NULL;
 for(int i=1;i<argc;++i) {
  if(!strcmp(argv[i],"--hd-smoke"))smoke=96;
  else if(!strcmp(argv[i],"--hd-capture") && i+1<argc)capture=argv[++i];
 }
 if(pop_hd_assets_check())return 1;
 if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS)){fprintf(stderr,"%s\n",SDL_GetError());return 1;}
 SDL_Window *window=SDL_CreateWindow("PoPHD art preview",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720,SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
 if(!window){SDL_Quit();return 1;}
 SDL_Renderer *renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
 if(!renderer)renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
 if(!renderer){SDL_DestroyWindow(window);SDL_Quit();return 1;}
 SDL_RenderSetLogicalSize(renderer,1280,800);
 SDL_Surface *rs=read_surface("data/hd-preview/room.png"),*ss=read_surface("data/hd-preview/run.png");
 SDL_Texture *room=rs?SDL_CreateTextureFromSurface(renderer,rs):NULL;
 SDL_Texture *hero=ss?SDL_CreateTextureFromSurface(renderer,ss):NULL;
 SDL_FreeSurface(rs);SDL_FreeSurface(ss);
 SDL_Texture *native[8]={0};int nw[8]={0},nh[8]={0},headx[8]={0};
 int failure=!room || !hero;
 for(int i=0;i<8;++i) {
  char name[100];snprintf(name,sizeof(name),"data/KID/res%d.png",407+i);
  SDL_Surface *s=read_surface(name);if(!s){failure=1;continue;}
  SDL_SetColorKey(s,SDL_TRUE,0);nw[i]=s->w;nh[i]=s->h;
  SDL_Surface *rgba=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_RGBA32,0);
  long sum=0,count=0;
  if(rgba)for(int y=0;y<6 && y<rgba->h;++y)for(int x=0;x<rgba->w;++x) {
   Uint8 r,g,b,a;SDL_GetRGBA(((Uint32*)((Uint8*)rgba->pixels+y*rgba->pitch))[x],rgba->format,&r,&g,&b,&a);
   if(a>127){sum+=x;++count;}
  }
  headx[i]=count?(int)(4*sum/count):nw[i]*2;
  native[i]=SDL_CreateTextureFromSurface(renderer,s);SDL_FreeSurface(rgba);SDL_FreeSurface(s);
  if(!native[i])failure=1;
  else SDL_SetTextureScaleMode(native[i],SDL_ScaleModeNearest);
 }
 if(failure)goto cleanup;
 SDL_SetTextureScaleMode(room,SDL_ScaleModeLinear);
 SDL_SetTextureScaleMode(hero,SDL_ScaleModeLinear);
 SDL_SetTextureBlendMode(hero,SDL_BLENDMODE_BLEND);
 int running=1,paused=0,slow=0,compare=0,zoom=0,travel=0,direction=-1;
 unsigned frame=0,seen=0,iterations=0;float x=900;
 Uint32 previous=SDL_GetTicks();double accumulator=0;
 /* DOS dx units are two pixels, each logical pixel is drawn at 4x. */
 const unsigned stride[8]={56,40,8,16,32,40,16,24};
 /* Test input comes from the separate SDL observer, never from gameplay. */
 while (running) {
  SDL_Event e;
  while(SDL_PollEvent(&e)) {
   if(e.type==SDL_QUIT)running=0;
   if(e.type!=SDL_KEYDOWN || e.key.repeat)continue;
   switch(e.key.keysym.sym) {
   case SDLK_ESCAPE:running=0;break;
   case SDLK_SPACE:paused=!paused;break;
   case SDLK_s:slow=!slow;break;
   case SDLK_TAB:compare=!compare;break;
   case SDLK_z:zoom=!zoom;break;
   case SDLK_a:travel=!travel;break;
   case SDLK_LEFT:direction=-1;travel=1;break;
   case SDLK_RIGHT:direction=1;travel=1;break;
   case SDLK_COMMA:paused=1;frame=(frame+7)%8;break;
   case SDLK_PERIOD:paused=1;frame=(frame+1)%8;break;
   case SDLK_HOME:frame=0;x=900;travel=0;break;
   default:break;
   }
  }
  Uint32 now=SDL_GetTicks(),elapsed=now-previous;previous=now;
  if(elapsed>250)elapsed=250;
  double period=slow?333.333:83.333;
  if(!paused)accumulator+=smoke?period:elapsed;
  while(accumulator>=period && !paused) {
   accumulator-=period;frame=(frame+1)%8;
   if(travel) {
    x+=direction*(float)stride[frame];
    if(x<780){x=780;direction=1;}if(x>1010){x=1010;direction=-1;}
   }
  }
  seen|=1u<<frame;
  SDL_SetRenderDrawColor(renderer,8,12,18,255);SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer,room,NULL,NULL);
  SDL_Rect src={(int)(frame%4)*CELL_W,(int)(frame/4)*CELL_H,CELL_W,CELL_H};
  SDL_RendererFlip flip=direction>0?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE;
  SDL_Rect target={(int)x-(direction>0?CELL_W-96:96),756-192,CELL_W,CELL_H};
  if(compare) {
   int width=nw[frame]*4,height=nh[frame]*4;
   SDL_Rect nt={(int)x-(direction>0?width-headx[frame]:headx[frame]),756-height,width,height};
   SDL_RenderCopyEx(renderer,native[frame],NULL,&nt,0,NULL,flip);
  } else SDL_RenderCopyEx(renderer,hero,&src,&target,0,NULL,flip);
  if(zoom) {
   SDL_Rect panel={600,70,590,450};SDL_SetRenderDrawColor(renderer,12,17,25,245);SDL_RenderFillRect(renderer,&panel);
   SDL_Rect enlarged={650,90,512,416};
   SDL_RenderCopyEx(renderer,hero,&src,&enlarged,0,NULL,flip);
  }
  for(int i=0;i<8;++i) {
   SDL_Rect box={24+i*34,774,26,8};
   SDL_SetRenderDrawColor(renderer,i==(int)frame?245:60,i==(int)frame?183:73,i==(int)frame?80:90,255);
   SDL_RenderFillRect(renderer,&box);
  }
  char title[300];snprintf(title,sizeof(title),
   "HD ROOM / %s HERO | pose %u/8 | %s | Tab DOS/HD | Space pause | S slow | , . step | A move | arrows face | Z zoom | Esc",
   compare?"DOS":"HD",frame+1,paused?"PAUSED":slow?"3 fps":"12 fps");
  SDL_SetWindowTitle(window,title);
  ++iterations;
  if(smoke && iterations>=smoke)running=0;
  if(!running && capture)failure|=save_view(renderer,capture);
  SDL_RenderPresent(renderer);
  SDL_Delay(smoke?0:4);
 }
 printf("HD preview: %u frames, pose mask=0x%02x, %s renderer, %s\n",iterations,seen,SDL_GetCurrentVideoDriver(),failure?"failed":"ok");
 if(smoke && seen!=255)failure=1;
cleanup:
 for(int i=0;i<8;++i)if(native[i])SDL_DestroyTexture(native[i]);
 if(hero)SDL_DestroyTexture(hero);
 if(room)SDL_DestroyTexture(room);
 SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();
 return failure;
}
