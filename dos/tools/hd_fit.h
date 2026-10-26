/* Isotropic, alpha-aware sprite resampling; preserves pose proportions. */
#ifndef POP_HD_FIT_H
#define POP_HD_FIT_H
#include <SDL.h>
#include <math.h>
static int hd_fit(SDL_Surface*s,SDL_Surface*t) {
 const double scale=(double)(t->h-2)/s->h;
 const double width=s->w*scale,left=(t->w-width)/2.0;
 if(width>t->w-2.0)return -1; /* Expand art bounds rather than squash anatomy. */
 for(int y=1;y<t->h-1;y++)for(int x=(int)floor(left);x<(int)ceil(left+width);x++){
  if(x<0||x>=t->w)continue;
  double x0=fmax(0,(x-left)/scale),x1=fmin(s->w,(x+1-left)/scale);
  double y0=(y-1)/scale,y1=fmin(s->h,y/scale);
  double a=0,r=0,g=0,b=0,total=1.0/(scale*scale);
  if(x1<=x0||y1<=y0)continue;
  for(int yy=(int)y0;yy<(int)ceil(y1)&&yy<s->h;yy++)for(int xx=(int)x0;xx<(int)ceil(x1)&&xx<s->w;xx++){
   double weight=(fmin(x1,xx+1)-fmax(x0,xx))*(fmin(y1,yy+1)-fmax(y0,yy));
   Uint32 v=((Uint32*)((Uint8*)s->pixels+yy*s->pitch))[xx];double wa=weight*(v>>24);
   a+=wa;r+=wa*((v>>16)&255);g+=wa*((v>>8)&255);b+=wa*(v&255);
  }
  if(a>0.001){unsigned aa=(unsigned)(a/total+0.5);if(aa>255)aa=255;
   ((Uint32*)((Uint8*)t->pixels+y*t->pitch))[x]=(aa<<24)|((unsigned)(r/a+0.5)<<16)|((unsigned)(g/a+0.5)<<8)|(unsigned)(b/a+0.5);
  }
 }
 return 0;
}
#endif
