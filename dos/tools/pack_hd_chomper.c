/* Fit painted steel and blood to the original moving jaw masks. Resource
 * sizes, drawing origins, frame order and DOS masks remain unchanged. */
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <stdio.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void put(SDL_Surface*s,int x,int y,Uint32 p){((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x]=p;}
static SDL_Surface*load(const char*p){SDL_Surface*s=IMG_Load(p);if(!s){fprintf(stderr,"%s: %s\n",p,IMG_GetError());exit(1);}SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);if(!r)exit(1);return r;}
/* Premultiplied bilinear sampling avoids dark fringes around liquid. */
static Uint32 sample(SDL_Surface*s,double x,double y){
 int ix=floor(x),iy=floor(y);double a=0,r=0,g=0,b=0;
 for(int j=0;j<2;j++)for(int i=0;i<2;i++){
  int xx=ix+i,yy=iy+j;xx=SDL_clamp(xx,0,s->w-1);yy=SDL_clamp(yy,0,s->h-1);
  Uint32 p=px(s,xx,yy);double w=(i?x-ix:1-x+ix)*(j?y-iy:1-y+iy)*(p>>24);
  a+=w;r+=w*((p>>16)&255);g+=w*((p>>8)&255);b+=w*(p&255);
 }if(a<.01)return 0;return ((Uint32)(a+.5)<<24)|((Uint32)(r/a+.5)<<16)|((Uint32)(g/a+.5)<<8)|(Uint32)(b/a+.5);
}
static int kind(Uint32 p,int palace){
 p&=0xffffff;if(!p)return 0;
 if(p==0xfcfcfc||p==(palace?0xc8bcb0:0xa8a8a8))return 1;
 if(p==(palace?0x4c4038:0x545454))return 2;
 return 3;
}
static double coverage(SDL_Surface*n,double x,double y,int category,int palace){
 int ix=floor(x),iy=floor(y);double a=0;
 for(int j=0;j<2;j++)for(int i=0;i<2;i++){
  int xx=SDL_clamp(ix+i,0,n->w-1),yy=SDL_clamp(iy+j,0,n->h-1);
  if(kind(px(n,xx,yy),palace)==category)a+=(i?x-ix:1-x+ix)*(j?y-iy:1-y+iy);
 }return a;
}
int main(void){
 SDL_Surface*steel=load("data/hd-source/chomper-steel.png"),*blood=load("data/hd-source/chomper-blood.png");
 int l=blood->w,t=blood->h,r=0,b=0;
 for(int y=0;y<blood->h;y++)for(int x=0;x<blood->w;x++)if((px(blood,x,y)>>24)>=192){l=SDL_min(l,x);r=SDL_max(r,x);t=SDL_min(t,y);b=SDL_max(b,y);}
 if(r<=l||b<=t)return 1;
 for(int palace=0;palace<2;palace++){
  const char*stem=palace?"VPALACE":"VDUNGEON";char path[256];
  snprintf(path,sizeof(path),"data/hd/%s/res241.png",stem);SDL_Surface*floor=load(path);
  for(int id=301;id<=323;id++){
   snprintf(path,sizeof(path),"data/%s/res%d.png",stem,id);SDL_Surface*n=load(path);
   SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,n->w*4,n->h*4,32,SDL_PIXELFORMAT_ARGB8888);if(!out)return 1;
   int offset=id==311?37:id==312?47:id==313?50:0;
   for(int y=0;y<out->h;y++)for(int x=0;x<out->w;x++){
    double nx=(x+.5)/4-.5,ny=(y+.5)/4-.5;
    if(id>=314){
     /* All phases reveal the same bottom-anchored stain; foreground is
      * the same left strip, so layering never draws a second smear. */
     double mask=coverage(n,nx,ny,3,palace);
     int world_y=116-out->h+y;
     Uint32 p=sample(blood,l+(x+.5)*(r-l+1)/24.0-.5,t+(world_y+.5)*(b-t+1)/116.0-.5);
     put(out,x,y,((Uint32)((p>>24)*mask+.5)<<24)|(p&0xffffff));
    }else{
     double metal=coverage(n,nx,ny,1,palace),seam=coverage(n,nx,ny,2,palace),stone=coverage(n,nx,ny,3,palace);
     Uint32 p=sample(steel,(x+.5)*steel->w/128.0-.5,(240-offset*4-out->h+y+.5)*steel->h/240.0-.5);
     Uint32 f=sample(floor,x,floor->h-out->h+y);
     double a=metal+seam+stone,shade=metal+seam*.40;Uint32 color=0;
     for(int shift=0;shift<=16;shift+=8){int c=a?(((p>>shift)&255)*shade+((f>>shift)&255)*stone)/a:0;color|=(Uint32)SDL_clamp(c,0,255)<<shift;}
     put(out,x,y,((Uint32)(255*a+.5)<<24)|color);
    }
   }
   snprintf(path,sizeof(path),"data/hd/%s/res%d.png",stem,id);if(IMG_SavePNG(out,path))return 1;
   SDL_FreeSurface(n);SDL_FreeSurface(out);
  }SDL_FreeSurface(floor);
 }
 SDL_FreeSurface(steel);SDL_FreeSurface(blood);puts("HD chomper: 46 painted resources packed");return 0;
}
