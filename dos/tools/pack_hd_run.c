/* Technical atlas extraction, registration and scale normalization.
 * New artwork is generated with imagegen; this tool does not synthesize art. */
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {int label,x0,y0,x1,y1,area;} Part;
static int order(const void *a,const void *b) {
 const Part *x=a,*y=b;
 int rowx=(x->y0+x->y1)/2/512,rowy=(y->y0+y->y1)/2/512;
 return rowx!=rowy?rowx-rowy:x->x0-y->x0;
}
int main(void) {
 SDL_Surface *input=IMG_Load("data/hd-preview/run-source.png");
 if(!input){fprintf(stderr,"%s\n",IMG_GetError());return 1;}
 SDL_Surface *s=SDL_ConvertSurfaceFormat(input,SDL_PIXELFORMAT_RGBA32,0);
 SDL_FreeSurface(input);if(!s)return 1;
 int w=s->w,h=s->h,n=w*h;
 unsigned char *alpha=malloc((size_t)n);int *labels=calloc((size_t)n,sizeof(int));
 int *queue=malloc((size_t)n*sizeof(int));if(!alpha || !labels || !queue)return 1;
 for(int y=0;y<h;++y)for(int x=0;x<w;++x) {
  Uint8 r,g,b,a;SDL_GetRGBA(((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x],s->format,&r,&g,&b,&a);
  alpha[y*w+x]=a;
 }
 Part parts[64];int count=0,label=0;
 for(int seed=0;seed<n;++seed) {
  if(alpha[seed]<32 || labels[seed])continue;
  ++label;Part part={label,w,h,0,0,0};int head=0,tail=0;
  queue[tail++]=seed;labels[seed]=label;
  while(head<tail) {
   int v=queue[head++],x=v%w,y=v/w;
   if(x<part.x0)part.x0=x;
   if(x>part.x1)part.x1=x;
   if(y<part.y0)part.y0=y;
   if(y>part.y1)part.y1=y;
   ++part.area;
   int near[4]={x>0?v-1:-1,x+1<w?v+1:-1,y>0?v-w:-1,y+1<h?v+w:-1};
   for(int k=0;k<4;++k)if(near[k]>=0 && alpha[near[k]]>=32 && !labels[near[k]]) {
    labels[near[k]]=label;queue[tail++]=near[k];
   }
  }
  if(part.area>8000) {
   if(count>=64){fputs("Too many components\n",stderr);return 1;}
   parts[count++]=part;
  }
 }
 if(count!=8){fprintf(stderr,"Expected 8 full figures, got %d\n",count);return 1;}
 qsort(parts,8,sizeof(Part),order);
 SDL_Surface *atlas=SDL_CreateRGBSurfaceWithFormat(0,1024,416,32,SDL_PIXELFORMAT_RGBA32);
 if(!atlas)return 1;
 SDL_FillRect(atlas,NULL,0);
 FILE *manifest=fopen("data/hd-preview/run-manifest.txt","w");if(!manifest)return 1;
 fputs("8 cells: 256x208; torso/head anchor x=96; feet y=192; frames DOS 7..14\n",manifest);
 for(int i=0;i<8;++i) {
  Part a=parts[i];int pw=a.x1-a.x0+1,ph=a.y1-a.y0+1;
  SDL_Surface *cut=SDL_CreateRGBSurfaceWithFormat(0,pw+4,ph+4,32,SDL_PIXELFORMAT_RGBA32);
  if(!cut)return 1;
  SDL_FillRect(cut,NULL,0);
  /* Preserve only this component and its immediate antialiased edge, not
   * neighboring poses which may cross nominal generated cell boundaries. */
  for(int y=a.y0;y<=a.y1;++y)for(int x=a.x0;x<=a.x1;++x) {
   int v=y*w+x,keep=labels[v]==a.label;
   if(!keep && alpha[v])for(int dy=-1;dy<=1 && !keep;++dy)for(int dx=-1;dx<=1;++dx) {
    int xx=x+dx,yy=y+dy;
    if(xx>=0 && xx<w && yy>=0 && yy<h && labels[yy*w+xx]==a.label)keep=1;
   }
   if(keep)((Uint32*)((Uint8*)cut->pixels+(y-a.y0+2)*cut->pitch))[x-a.x0+2]=
    ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];
  }
  /* Register each frame by the head rather than stride-dependent bounding
   * box center; this prevents horizontal wobble as the legs extend. */
  long sx=0,pixels=0;int headheight=ph/8;
  for(int y=a.y0;y<a.y0+headheight;++y)for(int x=a.x0;x<=a.x1;++x)
   if(labels[y*w+x]==a.label){sx+=x-a.x0+2;++pixels;}
  double headx=pixels?(double)sx/pixels:(pw+4)/2.0;
  double scale=168.0/ph;
  int dw=(int)((pw+4)*scale+0.5),dh=(int)((ph+4)*scale+0.5);
  int left=96-(int)(headx*scale+0.5),top=192-dh+1;
  if(left<0 || left+dw>256 || top<0){fprintf(stderr,"Frame %d does not fit\n",i);return 1;}
  SDL_Rect target={(i%4)*256+left,(i/4)*208+top,dw,dh};
  if(SDL_BlitScaled(cut,NULL,atlas,&target)){fprintf(stderr,"%s\n",SDL_GetError());return 1;}
  fprintf(manifest,"frame %d: source %d %d %d %d area=%d target=%d %d %d %d\n",
   i+7,a.x0,a.y0,pw,ph,a.area,left,top,dw,dh);
  SDL_FreeSurface(cut);
 }
 fclose(manifest);
 int result=IMG_SavePNG(atlas,"data/hd-preview/run.png");
 fprintf(stderr,"HD atlas: 8 distinct connected figures, head registration, 168px height, %s\n",result?"FAILED":"ok");
 SDL_FreeSurface(atlas);SDL_FreeSurface(s);free(alpha);free(labels);free(queue);
 return result!=0;
}
