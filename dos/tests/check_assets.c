/* Decode every shipped PNG and verify level captures contain actual graphics. */
#include <SDL.h>
#include <SDL_image.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
static unsigned images,indexed,captures,failed;
static void scan(const char *directory,int screenshots) {
 DIR *dir=opendir(directory);if(!dir){++failed;return;}
 struct dirent *entry;
 while((entry=readdir(dir))) {
  if(entry->d_name[0]=='.')continue;
  char path[4096];snprintf(path,sizeof(path),"%s/%s",directory,entry->d_name);
  struct stat info;if(stat(path,&info)){++failed;continue;}
  if(S_ISDIR(info.st_mode)){scan(path,screenshots);continue;}
  size_t len=strlen(path);if(len<4 || strcmp(path+len-4,".png"))continue;
  SDL_Surface *image=IMG_Load(path);
  if(!image){fprintf(stderr,"%s: %s\n",path,IMG_GetError());++failed;continue;}
  ++images;indexed+=image->format->palette!=NULL;
  if(screenshots) {
   ++captures;SDL_Surface *rgb=SDL_ConvertSurfaceFormat(image,SDL_PIXELFORMAT_ARGB8888,0);
   if(!rgb){++failed;SDL_FreeSurface(image);continue;}
   Uint32 first=*(Uint32*)rgb->pixels;unsigned changed=0;
   for(int y=0;y<rgb->h;++y)for(int x=0;x<rgb->w;++x)
    changed+=((Uint32*)((Uint8*)rgb->pixels+y*rgb->pitch))[x]!=first;
   if(rgb->w<320 || rgb->h<200 || changed<1000) {
    fprintf(stderr,"Empty or undersized capture: %s\n",path);++failed;
   }
   SDL_FreeSurface(rgb);
  }
  SDL_FreeSurface(image);
 }
 closedir(dir);
}
int main(void) {
 scan("data",0);scan("screenshots",1);
 printf("PNG assets/captures=%u indexed=%u captures=%u failures=%u\n",images,indexed,captures,failed);
 return failed || captures<15 || images<100;
}
