#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
int main(void){
 for(int g=0;g<3;g++){
 int count=g<2?16:2,cols=g<2?4:2,rows=g<2?4:1;
 SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,cols*384,rows*384,32,SDL_PIXELFORMAT_ARGB8888);SDL_FillRect(out,NULL,0);
 for(int k=0;k<count;k++){
  int id=751+g*16+k;char p[128];snprintf(p,sizeof(p),"data/GUARD/res%d.png",id);
  SDL_Surface*s=IMG_Load(p);if(!s)return 1;SDL_SetColorKey(s,SDL_TRUE,0);SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);
  double scale=4;if(r->w*scale>344)scale=344.0/r->w;if(r->h*scale>344)scale=344.0/r->h;
  SDL_Rect dst={(k%cols)*384+(384-r->w*scale)/2,(k/cols)*384+(384-r->h*scale)/2,r->w*scale,r->h*scale};
  SDL_BlitScaled(r,NULL,out,&dst);SDL_FreeSurface(r);
 }
 char path[128];snprintf(path,sizeof(path),"build/guard-ref-%d.png",g);IMG_SavePNG(out,path);SDL_FreeSurface(out);
 }
 return 0;
}