/* Pack painted modular dungeon pieces into the unchanged resource rectangles.
 * Only crop/resample artwork; original game data and collision masks stay intact. */
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void put(SDL_Surface*s,int x,int y,Uint32 p){((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x]=p;}
static SDL_Surface*load(const char*p){SDL_Surface*s=IMG_Load(p);if(!s){fprintf(stderr,"%s: %s\n",p,IMG_GetError());return NULL;}SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return r;}
static SDL_Surface*canvas(int w,int h){SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);SDL_FillRect(s,NULL,0);return s;}
static Uint32 sample(SDL_Surface*s,double x,double y){
 int ix=(int)floor(x),iy=(int)floor(y);double a=0,r=0,g=0,b=0;
 for(int j=0;j<2;j++)for(int i=0;i<2;i++){
  int xx=ix+i,yy=iy+j;if(xx<0)xx=0;if(xx>=s->w)xx=s->w-1;if(yy<0)yy=0;if(yy>=s->h)yy=s->h-1;
  Uint32 p=px(s,xx,yy);double w=(i?x-ix:1-x+ix)*(j?y-iy:1-y+iy)*(p>>24);
  a+=w;r+=w*((p>>16)&255);g+=w*((p>>8)&255);b+=w*(p&255);
 }if(a<.01)return 0;return ((Uint32)(a+.5)<<24)|((Uint32)(r/a+.5)<<16)|((Uint32)(g/a+.5)<<8)|(Uint32)(b/a+.5);
}
int main(void){
 SDL_Surface*sheet=load("data/hd-source/dungeon-architecture-cutout.png");if(!sheet)return 1;
 int ids[]={237,238,239,240,286,287,288,289,290,291};
 FILE*report=fopen("data/hd/architecture-manifest.txt","w");if(!report)return 1;
 for(int k=0;k<10;k++){
  /* The source preserves the reference sheet's five columns and two rows.
   * Start below the labels, and retain only substantial opaque art. */
  int l=sheet->w,t=sheet->h,r=0,b=0;
  int x0=k%5*sheet->w/5,x1=(k%5+1)*sheet->w/5;
  int y0=(k/5*250+25)*sheet->h/500,y1=(k/5*250+240)*sheet->h/500;
  for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++)if((px(sheet,x,y)>>24)>=192){if(x<l)l=x;if(x>r)r=x;if(y<t)t=y;if(y>b)b=y;}
  if(r<=l||b<=t)return 1;
  SDL_Surface*cut=canvas(r-l+1,b-t+1);
  for(int y=0;y<cut->h;y++)for(int x=0;x<cut->w;x++){Uint32 p=px(sheet,x+l,y+t);put(cut,x,y,(p>>24)>=192?p|0xff000000u:0);}
  char path[256];snprintf(path,sizeof(path),"data/VDUNGEON/res%d.png",ids[k]);SDL_Surface*n=load(path);if(!n)return 1;
  int nl=n->w,nt=n->h,nr=0,nb=0;
  for(int y=0;y<n->h;y++)for(int x=0;x<n->w;x++)if(px(n,x,y)&0xffffff){if(x<nl)nl=x;if(x>nr)nr=x;if(y<nt)nt=y;if(y>nb)nb=y;}
  SDL_Surface*out=canvas(n->w*4,n->h*4);
  for(int y=nt*4;y<(nb+1)*4;y++)for(int x=nl*4;x<(nr+1)*4;x++)
   put(out,x,y,sample(cut,(x-nl*4+.5)*cut->w/((nr-nl+1)*4)-.5,(y-nt*4+.5)*cut->h/((nb-nt+1)*4)-.5));
  /* Keep the original matte platform surfaces below the painted ornament.
   * Matching native palette colors identify floor facets, not column stone. */
  if(ids[k]==237||ids[k]==239){
   snprintf(path,sizeof(path),"data/hd-source/architecture-base/res%d.png",ids[k]);SDL_Surface*base=load(path);if(!base)return 1;
   for(int y=188;y<out->h;y++)for(int x=0;x<out->w;x++){
    Uint32 rgb=px(n,x/4,y/4)&0xffffff;
    if(rgb==0x445c70||rgb==0x30485c||rgb==0x1c304c||!rgb)put(out,x,y,px(base,x,y));
   }
   SDL_FreeSurface(base);
  }
  snprintf(path,sizeof(path),"data/hd/VDUNGEON/res%d.png",ids[k]);if(IMG_SavePNG(out,path))return 1;
  fprintf(report,"VDUNGEON %d source=%d,%d,%d,%d output=%dx%d\n",ids[k],l,t,r-l+1,b-t+1,out->w,out->h);
  SDL_FreeSurface(cut);SDL_FreeSurface(n);SDL_FreeSurface(out);
 }
 SDL_FreeSurface(sheet);
 /* Fit every scanline to one continuous native profile. Limiting this to
  * a short strip at the join creates another step at the strip's far end. */
 const int shafts[]={286,287,288,289,291};
 for(int k=0;k<5;k++){
  char path[256];snprintf(path,sizeof(path),"data/VDUNGEON/res%d.png",shafts[k]);SDL_Surface*n=load(path);if(!n)return 1;
  snprintf(path,sizeof(path),"data/hd/VDUNGEON/res%d.png",shafts[k]);SDL_Surface*a=load(path);if(!a)return 1;
  SDL_Surface*copy=SDL_ConvertSurface(a,a->format,0);if(!copy)return 1;
  int left[256],right[256];if(n->h>256)return 1;
  for(int y=0;y<n->h;y++){
   left[y]=n->w;right[y]=-1;
   for(int x=0;x<n->w;x++)if(px(n,x,y)&0xffffff){if(x<left[y])left[y]=x;right[y]=x;}
  }
  for(int y=0;y<a->h;y++){
   int l=a->w,r=-1;
   for(int x=0;x<a->w;x++)if((px(copy,x,y)>>24)>128){if(x<l)l=x;r=x;}
   if(r<=l)continue;
   double yy=(y+.5)/4-.5;int y0=(int)floor(yy);double f=yy-y0;int y1=y0+1;
   if(y0<0)y0=0;
   if(y1>=n->h)y1=n->h-1;
   if(right[y0]<left[y0]||right[y1]<left[y1])continue;
   double lo=4*((1-f)*left[y0]+f*left[y1]);
   double hi=4*((1-f)*(right[y0]+1)+f*(right[y1]+1));
   for(int x=0;x<a->w;x++){
    double coverage=fmax(0,fmin(x+1,hi)-fmax(x,lo));
    if(!coverage){put(a,x,y,0);continue;}
    Uint32 color=sample(copy,l+(x+.5-lo)*(r-l+1)/(hi-lo)-.5,y);
    put(a,x,y,((Uint32)(255*coverage+.5)<<24)|(color&0xffffff));
   }
  }
  if(IMG_SavePNG(a,path))return 1;
  SDL_FreeSurface(copy);SDL_FreeSurface(a);SDL_FreeSurface(n);
 }
 /* Sample a single painted shaft strip through both halves. The shared
  * vertical phase prevents a material seam where the two native tiles join. */
 SDL_Surface*shaft=load("data/hd/VDUNGEON/res291.png");if(!shaft)return 1;
 const int ids2[]={286,287,288,289,291},starts[]={0,0,0,72,80},ends[]={152,140,152,252,252};
 for(int k=0;k<5;k++){
  char path[256];snprintf(path,sizeof(path),"data/hd/VDUNGEON/res%d.png",ids2[k]);SDL_Surface*a=load(path);if(!a)return 1;
  for(int y=starts[k];y<ends[k];y++){
   int l=a->w,r=-1;for(int x=0;x<a->w;x++)if((px(a,x,y)>>24)>128){if(x<l)l=x;r=x;}
   if(r<=l)continue;
   int phase=(y+(k<3?252:0))%240;if(phase>=120)phase=239-phase;
   double blend=k<3?fmin(1,(ends[k]-y)/16.0):fmin(1,(y-starts[k]+1)/16.0);
   double dark=(k==1||k==3)?.78:1.0;
   for(int x=l;x<=r;x++){
    Uint32 old=px(a,x,y),paint=sample(shaft,24+(x-l)*43.0/(r-l),100+phase);
    Uint32 p=old&0xff000000u;
    for(int shift=0;shift<=16;shift+=8)p|=(Uint32)((1-blend)*((old>>shift)&255)+blend*dark*((paint>>shift)&255)+.5)<<shift;
    put(a,x,y,p);
   }
  }
  if(IMG_SavePNG(a,path))return 1;
  SDL_FreeSurface(a);
 }
 SDL_FreeSurface(shaft);
 /* The front overlay is the complete column body. Compose the base tile
  * from that SAME body and the ordinary floor, never warp both together. */
 SDL_Surface*front=load("data/hd/VDUNGEON/res288.png");if(!front)return 1;
 for(int id=286;id<=287;id++){
  char path[256];snprintf(path,sizeof(path),"data/VDUNGEON/res%d.png",id);SDL_Surface*n=load(path);if(!n)return 1;
  snprintf(path,sizeof(path),"data/VDUNGEON/res%d.png",id==286?241:242);SDL_Surface*nf=load(path);if(!nf)return 1;
  snprintf(path,sizeof(path),"data/hd/VDUNGEON/res%d.png",id==286?241:242);SDL_Surface*floor_art=load(path);if(!floor_art)return 1;
  SDL_Surface*a=canvas(n->w*4,n->h*4);
  for(int y=188;y<a->h;y++)for(int x=0;x<a->w;x++)put(a,x,y,px(floor_art,x,y-188));
  if(id==286){
   SDL_SetSurfaceBlendMode(front,SDL_BLENDMODE_BLEND);SDL_Rect at={32,0,0,0};SDL_BlitSurface(front,NULL,a,&at);
  }else{
   /* The rear body is a complete, centered pedestal on the floor plane.
    * Native 287 also contains a diagonal floor side: using its silhouette
    * for the column itself turns the foot into a wedge. Compress the round
    * painted foot in depth, retaining the shaft's exact 44px join width. */
   for(int y=0;y<208;y++){
    double sy=fmin(front->h-1,y+32*fmax(0,fmin(1,(y-112)/16.0)));
    int row=(int)sy,sl=front->w,sr=-1;
    for(int x=0;x<front->w;x++)if((px(front,x,row)>>24)>128){if(x<sl)sl=x;sr=x;}
    if(sr<=sl)continue;
    double width=44+((sr-sl+1)-44)/3.0,lo=30-width/2,hi=30+width/2;
    for(int x=(int)floor(lo);x<(int)ceil(hi)&&x<a->w;x++){
     if(x<0)continue;
     double coverage=fmax(0,fmin(x+1,hi)-fmax(x,lo));
     Uint32 paint=sample(front,sl+(x+.5-lo)*(sr-sl+1)/(hi-lo)-.5,sy),old=px(a,x,y),out=0xff000000u;
     for(int sh=0;sh<=16;sh+=8)out|=(Uint32)(coverage*.78*((paint>>sh)&255)+(1-coverage)*((old>>sh)&255)+.5)<<sh;
     if(!(old>>24))out=(out&0xffffff)|((Uint32)(255*coverage+.5)<<24);
     put(a,x,y,out);
    }
   }
  }
  snprintf(path,sizeof(path),"data/hd/VDUNGEON/res%d.png",id);if(IMG_SavePNG(a,path))return 1;
  SDL_FreeSurface(n);SDL_FreeSurface(nf);SDL_FreeSurface(floor_art);SDL_FreeSurface(a);
 }
 SDL_FreeSurface(front);
 /* The engine raises the exit in four-native-pixel steps and overlaps its
  * six-pixel slices. Use a 16-HD-pixel vertical period so all slices join. */
 SDL_Surface*gate=load("data/hd-source/dungeon-exit-surface.png");if(!gate)return 1;
 for(int id=233;id<=234;id++){
  SDL_Surface*out=canvas(168,24);
  for(int y=0;y<24;y++)for(int x=0;x<168;x++){
   double u=(x%16+.5)/16.0,v=(y%16+.5)/16.0;
   Uint32 p=sample(gate,gate->w*(.25+u*.25),gate->h*(.25+v*.25));
   if(x<8||x>=164||(id==234&&y<8)){
    unsigned c=(x<2||x>=166||(id==234&&y<2))?42:78;
    p=0xff000000u|(c<<16)|((c+15)<<8)|(c+29);
   }
   put(out,x,y,p);
  }
  char path[128];snprintf(path,sizeof(path),"data/hd/VDUNGEON/res%d.png",id);if(IMG_SavePNG(out,path))return 1;
  fprintf(report,"VDUNGEON %d painted gate surface output=168x24 vertical-period=16\n",id);SDL_FreeSurface(out);
 }
 SDL_FreeSurface(gate);fclose(report);return 0;
}
