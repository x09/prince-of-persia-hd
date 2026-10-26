#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
int main(void){
 int ids[256],n=0;
 for(int id=401;id<=616;id++){char p[128];snprintf(p,sizeof(p),"data/hd/KID/res%d.png",id);
 FILE*f=fopen(p,"rb");if(f){fclose(f);continue;}ids[n++]=id;}
 FILE*m=fopen("build/hero-missing-map.txt","w");if(!m)return 1;
 for(int g=0;g<(n+15)/16;g++){
 SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,1536,1536,32,SDL_PIXELFORMAT_RGBA32);if(!out)return 1;SDL_FillRect(out,NULL,0);
 for(int cell=0;cell<16;cell++){int ix=g*16+cell;int id=ids[ix<n?ix:n-1];char p[128];snprintf(p,sizeof(p),"data/KID/res%d.png",id);
 SDL_Surface*s=IMG_Load(p);if(!s)return 1;SDL_SetColorKey(s,SDL_TRUE,0);
 SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_RGBA32,0);SDL_FreeSurface(s);if(!r)return 1;
 double scale=4.0;if(r->w*scale>344)scale=344.0/r->w;if(r->h*scale>344)scale=344.0/r->h;
 int w=r->w*scale,h=r->h*scale;SDL_Rect dst={(cell%4)*384+(384-w)/2,(cell/4)*384+(384-h)/2,w,h};
 SDL_BlitScaled(r,NULL,out,&dst);SDL_FreeSurface(r);fprintf(m,"%d %d %d %d\n",g,cell,id,ix<n);
 }
 char p[128];snprintf(p,sizeof(p),"build/hero-ref-%02d.png",g);if(IMG_SavePNG(out,p))return 1;SDL_FreeSurface(out);
 }fclose(m);printf("missing=%d sheets=%d\n",n,(n+15)/16);return 0;
}