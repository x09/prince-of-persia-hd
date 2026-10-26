/* Inventory missing HD PNG assets and make ordered native contact sheets. */
#include <SDL.h>
#include <SDL_image.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int cmp(const void*a,const void*b){return *(const int*)a-*(const int*)b;}
int main(void){
 const char*stems[]={"KID","GUARD","FAT","SKEL","VIZIER","SHADOW","PRINCE","PV","TITLE","VDUNGEON","VPALACE"};
 FILE*f=fopen("build/hd-missing.tsv","w");if(!f)return 1;int total=0;
 for(unsigned g=0;g<sizeof(stems)/sizeof(stems[0]);g++){
  const char*stem=stems[g];char path[256];snprintf(path,sizeof(path),"data/%s",stem);DIR*d=opendir(path);if(!d)return 2;
  int ids[1024],count=0;struct dirent*e;
  while((e=readdir(d))){int id;char tail;if(sscanf(e->d_name,"res%d.png%c",&id,&tail)!=1||!strstr(e->d_name,".png"))continue;
   snprintf(path,sizeof(path),"data/hd/%s/res%d.png",stem,id);FILE*hd=fopen(path,"rb");if(hd){fclose(hd);continue;}ids[count++]=id;
  }closedir(d);qsort(ids,count,sizeof(int),cmp);if(!count)continue;
  int cols=count<6?count:6,rows=(count+cols-1)/cols;
  SDL_Surface*sheet=SDL_CreateRGBSurfaceWithFormat(0,cols*256,rows*256,32,SDL_PIXELFORMAT_ARGB8888);SDL_FillRect(sheet,NULL,0);
  for(int k=0;k<count;k++){
   snprintf(path,sizeof(path),"data/%s/res%d.png",stem,ids[k]);SDL_Surface*s=IMG_Load(path);if(!s)return 3;
   fprintf(f,"%s\t%d\t%d\t%d\t%d\n",stem,ids[k],s->w,s->h,k);
   SDL_SetColorKey(s,SDL_TRUE,0);SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);
   double scale=4;if(r->w*scale>230)scale=230.0/r->w;if(r->h*scale>230)scale=230.0/r->h;
   SDL_Rect target={(k%cols)*256+(256-r->w*scale)/2,(k/cols)*256+(256-r->h*scale)/2,r->w*scale,r->h*scale};
   SDL_BlitScaled(r,NULL,sheet,&target);SDL_FreeSurface(r);
  }
  snprintf(path,sizeof(path),"build/missing-%s.png",stem);IMG_SavePNG(sheet,path);SDL_FreeSurface(sheet);printf("%s missing=%d\n",stem,count);total+=count;
 }
 fclose(f);printf("TOTAL missing=%d\n",total);return 0;
}
