#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char**argv){
 if(argc!=7)return 1;
 const char*stem=argv[1];int first=atoi(argv[2]),count=atoi(argv[3]),cols=atoi(argv[4]),rows=atoi(argv[5]);
 SDL_Surface*out=SDL_CreateRGBSurfaceWithFormat(0,cols*256,rows*256,32,SDL_PIXELFORMAT_ARGB8888);SDL_FillRect(out,NULL,0);
 FILE*f=fopen("build/characters-native.txt","a");
 for(int k=0;k<count;k++){
  char p[256];snprintf(p,sizeof(p),"data/%s/res%d.png",stem,first+k);SDL_Surface*s=IMG_Load(p);if(!s)return 2;
  SDL_SetColorKey(s,SDL_TRUE,0);SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);
  double scale=4;if(r->w*scale>220)scale=220.0/r->w;if(r->h*scale>220)scale=220.0/r->h;
  SDL_Rect d={(k%cols)*256+(256-r->w*scale)/2,(k/cols)*256+(256-r->h*scale)/2,r->w*scale,r->h*scale};
  SDL_BlitScaled(r,NULL,out,&d);fprintf(f,"%s %d %dx%d\n",stem,first+k,r->w,r->h);SDL_FreeSurface(r);
 }
 fclose(f);int result=IMG_SavePNG(out,argv[6]);SDL_FreeSurface(out);return result?3:0;
}
