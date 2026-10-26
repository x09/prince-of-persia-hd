/* Packs generated artwork into the DOS resource geometry.
 * C/SDL2 only. Original data/ files are never overwritten. */
#include <SDL.h>
#include <SDL_image.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hd_fit.h"
typedef struct {int id,x0,y0,x1,y1,area;} Part;
static Uint32 pixel(SDL_Surface *s,int x,int y) {return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void put(SDL_Surface *s,int x,int y,Uint32 c) {((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x]=c;}
static SDL_Surface *load(const char *path,int key) {
 SDL_Surface *s=IMG_Load(path);if(!s){fprintf(stderr,"%s: %s\n",path,IMG_GetError());return NULL;}
 if(key)SDL_SetColorKey(s,SDL_TRUE,0);
 SDL_Surface *r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return r;
}
static int part_order(const void *a,const void *b) {
 const Part *x=a,*y=b;int rowx=(x->y0+x->y1)/1024,rowy=(y->y0+y->y1)/1024;
 return rowx!=rowy?rowx-rowy:x->x0-y->x0;
}
static int hero(const char *path,const int resources[8],FILE *report) {
 SDL_Surface *s=load(path,0);if(!s)return 1;
 int w=s->w,h=s->h,n=w*h,*labels=calloc((size_t)n,sizeof(int)),*queue=malloc((size_t)n*sizeof(int));
 if(!labels || !queue)return 1;
 Part parts[32];int count=0,id=0;
 for(int seed=0;seed<n;++seed) {
  if(labels[seed] || (pixel(s,seed%w,seed/w)>>24)<32)continue;
  Part part={++id,w,h,0,0,0};int head=0,tail=0;queue[tail++]=seed;labels[seed]=id;
  while(head<tail) {
   int v=queue[head++],x=v%w,y=v/w;
   if(x<part.x0)part.x0=x;
   if(x>part.x1)part.x1=x;
   if(y<part.y0)part.y0=y;
   if(y>part.y1)part.y1=y;
   ++part.area;
   int near[4]={x?v-1:-1,x+1<w?v+1:-1,y?v-w:-1,y+1<h?v+w:-1};
   for(int k=0;k<4;++k)if(near[k]>=0 && !labels[near[k]] && (pixel(s,near[k]%w,near[k]/w)>>24)>=32) {
    labels[near[k]]=id;queue[tail++]=near[k];
   }
  }
  if(part.area>3000){if(count>=32)return 1;parts[count++]=part;}
 }
 if(count!=8){fprintf(stderr,"%s: expected 8 figures, got %d\n",path,count);return 1;}
 qsort(parts,8,sizeof(Part),part_order);
 for(int i=0;i<8;++i) {
  char native_path[128],out[128];snprintf(native_path,sizeof(native_path),"data/KID/res%d.png",resources[i]);
  SDL_Surface *native=load(native_path,1);if(!native)return 1;
  Part a=parts[i];int sw=a.x1-a.x0+1,sh=a.y1-a.y0+1;
  SDL_Surface *cut=SDL_CreateRGBSurfaceWithFormat(0,sw,sh,32,SDL_PIXELFORMAT_ARGB8888);
  SDL_Surface *dest=SDL_CreateRGBSurfaceWithFormat(0,(native->w+32)*4,native->h*4,32,SDL_PIXELFORMAT_ARGB8888);
  if(!cut || !dest)return 1;
  SDL_FillRect(cut,NULL,0);SDL_FillRect(dest,NULL,0);
  for(int y=a.y0;y<=a.y1;++y)for(int x=a.x0;x<=a.x1;++x) {
   int keep=labels[y*w+x]==a.id;
   if(!keep && (pixel(s,x,y)>>24)>=8)for(int yy=y-1;yy<=y+1 && !keep;++yy)for(int xx=x-1;xx<=x+1;++xx)
    if(xx>=0 && xx<w && yy>=0 && yy<h && labels[yy*w+xx]==a.id)keep=1;
   if(keep)put(cut,x-a.x0,y-a.y0,pixel(s,x,y));
  }
  if(hd_fit(cut,dest)){fprintf(stderr,"hero %d exceeds padded bounds\n",resources[i]);return 1;}
  snprintf(out,sizeof(out),"data/hd/KID/res%d.png",resources[i]);
  if(IMG_SavePNG(dest,out))return 1;
  fprintf(report,"KID %d %dx%d new drawing from %s cell %d\n",resources[i],dest->w,dest->h,path,i);
  SDL_FreeSurface(native);SDL_FreeSurface(cut);SDL_FreeSurface(dest);
 }
 free(labels);free(queue);SDL_FreeSurface(s);return 0;
}
static void scale_mask(const unsigned char *src,int w,int h,unsigned char *dst) {
 for(int y=0;y<h;++y)for(int x=0;x<w;++x) {
  int b=src[(y?y-1:y)*w+x],e=src[y*w+x],d=src[y*w+(x?x-1:x)];
  int f=src[y*w+(x+1<w?x+1:x)],g=src[(y+1<h?y+1:y)*w+x];
  int p[4]={e,e,e,e};
  if(b!=g && d!=f){p[0]=d==b?d:e;p[1]=b==f?f:e;p[2]=d==g?d:e;p[3]=g==f?f:e;}
  for(int yy=0;yy<2;++yy)for(int xx=0;xx<2;++xx)dst[(y*2+yy)*w*2+x*2+xx]=(unsigned char)p[yy*2+xx];
 }
}
/* Targeted material rebuild preserves the painted bones and precision props
 * produced by later packers in make hd-assets. */
static int dungeon_only;
static int dungeon_override(int resource) {
 if(resource==230 || resource==231 || resource==233 || resource==234 ||
    (resource>=237 && resource<=240) || (resource>=286 && resource<=291) ||
    (resource>=301 && resource<=323))return 1;
 FILE *f=fopen("tools/hd_remaining.tsv","r");if(!f)return -1;
 char stem[64];int id,w,h,index,found=0;
 while(fscanf(f,"%63s %d %d %d %d",stem,&id,&w,&h,&index)==5)
  if(!strcmp(stem,"VDUNGEON") && id==resource){found=1;break;}
 fclose(f);return found;
}
static int environment(const char *name,int palace,SDL_Surface *material,FILE *report) {
 char dirpath[128];snprintf(dirpath,sizeof(dirpath),"data/%s",name);
 DIR *dir=opendir(dirpath);if(!dir)return -1;struct dirent *entry;int count=0;
 int mean_r=0,mean_g=0,mean_b=0;
 if(!palace){
  unsigned long long sr=0,sg=0,sb=0;
  for(int y=0;y<material->h;++y)for(int x=0;x<material->w;++x){Uint32 c=pixel(material,x,y);sr+=(c>>16)&255;sg+=(c>>8)&255;sb+=c&255;}
  unsigned long long n=(unsigned long long)material->w*material->h;
  mean_r=(int)(sr/n);mean_g=(int)(sg/n);mean_b=(int)(sb/n);
 }
 while((entry=readdir(dir))) {
  int resource=0;if(sscanf(entry->d_name,"res%d.png",&resource)!=1 || !strstr(entry->d_name,".png"))continue;
  if(dungeon_only){int skip=dungeon_override(resource);if(skip<0){closedir(dir);return -1;}if(skip)continue;}
  char path[512];snprintf(path,sizeof(path),"%s/%s",dirpath,entry->d_name);
  SDL_Surface *s=load(path,1);if(!s){closedir(dir);return -1;}
  int w=s->w,h=s->h;size_t n=(size_t)w*h;
  unsigned char *base=malloc(n),*twice=malloc(n*4),*mask=malloc(n*16);
  SDL_Surface *out=SDL_CreateRGBSurfaceWithFormat(0,w*4,h*4,32,SDL_PIXELFORMAT_ARGB8888);
  if(!base || !twice || !mask || !out)return -1;
  for(int y=0;y<h;++y)for(int x=0;x<w;++x)base[y*w+x]=(pixel(s,x,y)>>24)>=128;
  scale_mask(base,w,h,twice);scale_mask(twice,w*2,h*2,mask);unsigned modified=0;
  for(int y=0;y<h*4;++y)for(int x=0;x<w*4;++x) {
   if(!mask[y*w*4+x]){put(out,x,y,0);continue;}
   Uint32 c=pixel(s,x/4,y/4);
   if((c>>24)<128) {
    for(int dy=-1;dy<=1 && (c>>24)<128;++dy)for(int dx=-1;dx<=1;++dx) {
     int xx=x/4+dx,yy=y/4+dy;
     if(xx>=0 && xx<w && yy>=0 && yy<h && (pixel(s,xx,yy)>>24)>=128)c=pixel(s,xx,yy);
    }
   }
   int r=(c>>16)&255,g=(c>>8)&255,b=c&255;
   int stone=palace?(r>b+8 && r>40 && g>b-5):(b>r+4 && g>24 && b<220);
   if(stone) {
    /* Use the whole quiet dungeon material instead of repeating a 210x220
     * scratch-bearing crop. Palace sampling remains unchanged. */
    int tx=palace?material->w/2+180+(x+resource*17)%210:(x+resource*17)%material->w;
    int ty=palace?180+(y+resource*23)%220:(y+resource*23)%material->h;
    Uint32 m=pixel(material,tx,ty);int mr=(m>>16)&255,mg=(m>>8)&255,mb=m&255;
    /* Keep mineral pores subtle at the game's scale rather than turning
     * every pore into a bright repeated highlight. */
    if(!palace){mr=mean_r+(mr-mean_r)*2/5;mg=mean_g+(mg-mean_g)*2/5;mb=mean_b+(mb-mean_b)*2/5;}
    int lum=(r*3+g*6+b)/10;double shade=0.45+0.55*(lum/110.0);
    if(shade>1.3)shade=1.3;
    r=(int)(mr*shade);g=(int)(mg*shade);b=(int)(mb*shade);
    if(r>255)r=255;
    if(g>255)g=255;
    if(b>255)b=255;
    c=0xff000000u|((unsigned)r<<16)|((unsigned)g<<8)|(unsigned)b;++modified;
   }
   put(out,x,y,c);
  }
  if(modified) {
   snprintf(path,sizeof(path),"data/hd/%s/res%d.png",name,resource);
   if(IMG_SavePNG(out,path))return -1;
   fprintf(report,"%s %d %dx%d new material pixels=%u; DOS geometry mask\n",name,resource,w*4,h*4,modified);++count;
  }
  SDL_FreeSurface(s);SDL_FreeSurface(out);free(base);free(twice);free(mask);
 }
 closedir(dir);return count;
}

static int flames(FILE *report) {
 SDL_Surface *s=load("data/hd-source/fire.png",0);if(!s)return 1;
 for(int i=0;i<9;++i) {
  int cw=s->w/3,ch=s->h/3,ox=(i%3)*cw,oy=(i/3)*ch,n=cw*ch;
  unsigned char *seen=calloc(n,1);int *q=malloc(n*sizeof(int)),*best=malloc(n*sizeof(int)),bestn=0;
  if(!seen || !q || !best)return 1;
  for(int seed=0;seed<n;++seed) {
   if(seen[seed] || (pixel(s,ox+seed%cw,oy+seed/cw)>>24)<64)continue;
   int head=0,tail=0;q[tail++]=seed;seen[seed]=1;
   while(head<tail) {
    int v=q[head++],x=v%cw,y=v/cw;
    int near[4]={x?v-1:-1,x+1<cw?v+1:-1,y?v-cw:-1,y+1<ch?v+cw:-1};
    for(int k=0;k<4;++k)if(near[k]>=0 && !seen[near[k]] && (pixel(s,ox+near[k]%cw,oy+near[k]/cw)>>24)>=64) {
     seen[near[k]]=1;q[tail++]=near[k];
    }
   }
   if(tail>bestn){bestn=tail;memcpy(best,q,tail*sizeof(int));}
  }
  if(bestn<100)return 1;
  int x0=cw,y0=ch,x1=0,y1=0;
  for(int k=0;k<bestn;++k){int x=best[k]%cw,y=best[k]/cw;if(x<x0)x0=x;if(x>x1)x1=x;if(y<y0)y0=y;if(y>y1)y1=y;}
  SDL_Surface *cut=SDL_CreateRGBSurfaceWithFormat(0,x1-x0+1,y1-y0+1,32,SDL_PIXELFORMAT_ARGB8888);
  SDL_FillRect(cut,NULL,0);
  for(int k=0;k<bestn;++k){int x=best[k]%cw,y=best[k]/cw;put(cut,x-x0,y-y0,pixel(s,ox+x,oy+y));}
  char path[128];snprintf(path,sizeof(path),"data/PRINCE/res%d.png",151+i);
  SDL_Surface *native=load(path,1);if(!native)return 1;
  int nx0=native->w,ny0=native->h,nx1=0,ny1=0;
  for(int y=0;y<native->h;++y)for(int x=0;x<native->w;++x)if(pixel(native,x,y)>>24){
   if(x<nx0)nx0=x;
   if(x>nx1)nx1=x;
   if(y<ny0)ny0=y;
   if(y>ny1)ny1=y;
  }
  SDL_Surface *out=SDL_CreateRGBSurfaceWithFormat(0,native->w*4,native->h*4,32,SDL_PIXELFORMAT_ARGB8888);
  SDL_FillRect(out,NULL,0);SDL_Rect dst={nx0*4,ny0*4,(nx1-nx0+1)*4,(ny1-ny0+1)*4};
  SDL_BlitScaled(cut,NULL,out,&dst);
  snprintf(path,sizeof(path),"data/hd/PRINCE/res%d.png",151+i);
  if(IMG_SavePNG(out,path))return 1;
  fprintf(report,"PRINCE %d %dx%d new flame drawing\n",151+i,out->w,out->h);
  SDL_FreeSurface(cut);SDL_FreeSurface(native);SDL_FreeSurface(out);free(seen);free(q);free(best);
 }
 SDL_FreeSurface(s);return 0;
}

int main(int argc,char **argv) {
 if(argc==2 && !strcmp(argv[1],"--dungeon-only")) {
  dungeon_only=1;
  SDL_Surface *material=load("data/hd-source/dungeon-stone-matte.png",0);if(!material)return 1;
  FILE *report=fopen("data/hd/dungeon-material-manifest.txt","w");if(!report){SDL_FreeSurface(material);return 1;}
  int count=environment("VDUNGEON",0,material,report);
  fclose(report);SDL_FreeSurface(material);
  if(count<0)return 1;
  printf("HD dungeon material: %d resources; painted bones and precision props preserved\n",count);return 0;
 }
 FILE *report=fopen("data/hd/manifest.txt","w");if(!report)return 1;
 static const int running[8]={407,408,409,410,411,412,413,414};
 static const int start[8]={401,402,403,404,405,406,415,520};
 static const int air[8]={434,439,444,500,504,513,517,521};
 if(hero("data/hd-preview/run-source.png",running,report) ||
    hero("data/hd-source/poses-start.png",start,report) ||
    hero("data/hd-source/poses-air.png",air,report))return 1;
 SDL_Surface *material=load("data/hd-source/materials-soft.png",0);if(!material)return 1;
 int palace=environment("VPALACE",1,material,report);
 SDL_FreeSurface(material);
 material=load("data/hd-source/dungeon-stone-matte.png",0);if(!material)return 1;
 int dungeon=environment("VDUNGEON",0,material,report);
 SDL_FreeSurface(material);if(flames(report))return 1;fclose(report);
 printf("HD pack: hero=24, dungeon=%d, palace=%d\n",dungeon,palace);
 return dungeon<0 || palace<0;
}
