/* Reconstruct thin native sword axes at 4x with analytic antialiasing.
 * Keep original resource bounds and all game attachment offsets. */
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <stdio.h>
static int solid(SDL_Surface*s,int x,int y){
 Uint8*p=(Uint8*)s->pixels+y*s->pitch+x*s->format->BytesPerPixel;
 if(s->format->BytesPerPixel==1)return *p!=0;
 Uint32 v=0;SDL_memcpy(&v,p,s->format->BytesPerPixel);Uint8 r,g,b,a;
 SDL_GetRGBA(v,s->format,&r,&g,&b,&a);return a&& (r||g||b);
}
int main(void){
 for(int id=701;id<=734;id++){
  char path[128];snprintf(path,sizeof(path),"data/PRINCE/res%d.png",id);SDL_Surface*s=IMG_Load(path);
  if(!s)return 1;
  double cx=0,cy=0,n=0,xx=0,yy=0,xy=0;
  for(int y=0;y<s->h;y++)for(int x=0;x<s->w;x++)if(solid(s,x,y)){cx+=x+.5;cy+=y+.5;n++;}
  if(!n)return 2;
  cx/=n;cy/=n;
  for(int y=0;y<s->h;y++)for(int x=0;x<s->w;x++)if(solid(s,x,y)){double a=x+.5-cx,b=y+.5-cy;xx+=a*a;yy+=b*b;xy+=a*b;}
  double angle=.5*atan2(2*xy,xx-yy),dx=cos(angle),dy=sin(angle),lo=1e6,hi=-1e6;
  for(int y=0;y<s->h;y++)for(int x=0;x<s->w;x++)if(solid(s,x,y)){double t=(x+.5-cx)*dx+(y+.5-cy)*dy;if(t<lo)lo=t;if(t>hi)hi=t;}
  SDL_Surface*h=SDL_CreateRGBSurfaceWithFormat(0,s->w*4,s->h*4,32,SDL_PIXELFORMAT_ARGB8888);
  if(!h)return 3;
  for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++){
   int coverage=0;double shade=0;
   for(int j=0;j<4;j++)for(int i=0;i<4;i++){
    double a=(x+(i+.5)/4)/4-cx,b=(y+(j+.5)/4)/4-cy;
    double t=a*dx+b*dy,q=-a*dy+b*dx,end=t<lo?lo-t:t>hi?t-hi:0;
    if(q*q+end*end<=.46*.46){coverage++;shade+=q;}
   }
   Uint32 p=0;if(coverage){int light=205+(int)(shade/coverage*75);p=((Uint32)(coverage*255/16)<<24)|((light-15)<<16)|(light<<8)|(light+12);}
   ((Uint32*)((Uint8*)h->pixels+y*h->pitch))[x]=p;
  }
  snprintf(path,sizeof(path),"data/hd/PRINCE/res%d.png",id);if(IMG_SavePNG(h,path))return 4;
  SDL_FreeSurface(h);SDL_FreeSurface(s);
 }
 puts("HD swords: 34 resources, original bounds, antialiased steel axes");return 0;
}
