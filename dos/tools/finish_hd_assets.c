/* Full HD coverage: new scene paintings, outline text, procedural props
 * and antialiased material reconstruction of precision collision masks.
 * Does not claim reconstructed masks are newly hand-painted artwork. */
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vendor/stb_truetype.h"
static Uint32 px(SDL_Surface*s,int x,int y){return ((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x];}
static void put(SDL_Surface*s,int x,int y,Uint32 v){((Uint32*)((Uint8*)s->pixels+y*s->pitch))[x]=v;}
static SDL_Surface*rgba(const char*p,int key){SDL_Surface*s=IMG_Load(p);if(!s){fprintf(stderr,"%s: %s\n",p,IMG_GetError());return NULL;}if(key)SDL_SetColorKey(s,SDL_TRUE,0);SDL_Surface*r=SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);SDL_FreeSurface(s);return r;}
static int save(SDL_Surface*s,const char*stem,int id){char p[256];snprintf(p,sizeof(p),"data/hd/%s/res%d.png",stem,id);return IMG_SavePNG(s,p);}
static SDL_Surface*canvas(int w,int h){SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);if(s)SDL_FillRect(s,NULL,0);return s;}
static SDL_Surface*resize(SDL_Surface*s,int w,int h){
 SDL_Surface*out=canvas(w,h);if(!out)return NULL;
 for(int y=0;y<h;y++)for(int x=0;x<w;x++){
  double a=0,r=0,g=0,b=0;
  for(int j=0;j<4;j++)for(int i=0;i<4;i++){
   int xx=(int)((x+(i+.5)/4)*s->w/w),yy=(int)((y+(j+.5)/4)*s->h/h);
   Uint32 p=px(s,xx,yy);double aa=p>>24;a+=aa;r+=aa*((p>>16)&255);g+=aa*((p>>8)&255);b+=aa*(p&255);
  }
  if(a)put(out,x,y,((Uint32)(a/16+.5)<<24)|((Uint32)(r/a+.5)<<16)|((Uint32)(g/a+.5)<<8)|(Uint32)(b/a+.5));
 }return out;
}
static int scenes(void){
 SDL_Surface*s=rgba("data/hd-source/room.png",0);if(!s)return 1;SDL_Surface*room=resize(s,1280,800);SDL_FreeSurface(s);if(!room||save(room,"PV",951))return 1;
 SDL_Surface*p=canvas(64,192);SDL_Rect from={240,480,64,192};SDL_SetSurfaceBlendMode(room,SDL_BLENDMODE_NONE);SDL_BlitSurface(room,&from,p,NULL);if(save(p,"PV",952))return 1;SDL_FreeSurface(p);SDL_FreeSurface(room);
 s=rgba("data/hd-source/title.png",0);if(!s)return 1;SDL_Surface*title=resize(s,1280,800);SDL_FreeSurface(s);if(!title)return 1;
 s=rgba("data/hd-source/title-logo.png",0);if(!s)return 1;
 int ll=s->w,tt=s->h,rr=0,bb=0;
 for(int y=0;y<s->h;y++)for(int x=0;x<s->w;x++)if((px(s,x,y)>>24)>=128){if(x<ll)ll=x;if(x>rr)rr=x;if(y<tt)tt=y;if(y>bb)bb=y;}
 if(rr<=ll||bb<=tt)return 1;
 SDL_Surface*lc=canvas(rr-ll+1,bb-tt+1);SDL_Rect lr={ll,tt,lc->w,lc->h};
 SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_NONE);SDL_BlitSurface(s,&lr,lc,NULL);
 double ls=fmin(800.0/lc->w,292.0/lc->h);SDL_Surface*logo=resize(lc,(int)(lc->w*ls),(int)(lc->h*ls));
 if(!logo)return 1;
 SDL_Rect ld={700-logo->w/2,64,0,0};SDL_SetSurfaceBlendMode(logo,SDL_BLENDMODE_BLEND);SDL_BlitSurface(logo,NULL,title,&ld);
 SDL_FreeSurface(logo);SDL_FreeSurface(lc);SDL_FreeSurface(s);
 if(save(title,"TITLE",51))return 1;
 SDL_Rect middle={56,56,1168,632};SDL_FillRect(title,&middle,0xff000000);if(save(title,"TITLE",41))return 1;SDL_FreeSurface(title);
 s=rgba("data/hd-source/bed-empty.png",0);if(!s)return 1;
 int l=s->w,t=s->h,r=0,b=0;for(int y=0;y<s->h;y++)for(int x=0;x<s->w;x++)if((px(s,x,y)>>24)>192){if(x<l)l=x;if(x>r)r=x;if(y<t)t=y;if(y>b)b=y;}
 SDL_Surface*cut=canvas(r-l+1,b-t+1);SDL_Rect box={l,t,r-l+1,b-t+1};SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_NONE);SDL_BlitSurface(s,&box,cut,NULL);SDL_Surface*bed=canvas(960,164);
 double scale=fmin(958.0/cut->w,162.0/cut->h);SDL_Surface*fitted=resize(cut,(int)(cut->w*scale),(int)(cut->h*scale));if(!fitted)return 1;
 SDL_SetSurfaceBlendMode(fitted,SDL_BLENDMODE_NONE);SDL_Rect target={(960-fitted->w)/2,164-fitted->h,0,0};SDL_BlitSurface(fitted,NULL,bed,&target);SDL_FreeSurface(fitted);
 SDL_Surface*nativebed=rgba("data/PV/res981.png",1);if(!nativebed)return 1;
 int first=nativebed->w,last=-1,occupied[240]={0};
 for(int x=0;x<nativebed->w;x++)for(int y=0;y<nativebed->h;y++)if((px(nativebed,x,y)>>24)>128)occupied[x]=1;
 for(int x=0;x<nativebed->w;x++)if(occupied[x]){if(x<first)first=x;last=x;}
 for(int x=first+1;x<last;x++)if(!occupied[x])for(int y=0;y<bed->h;y++)for(int i=0;i<4;i++)put(bed,x*4+i,y,0);
 SDL_FreeSurface(nativebed);
 if(save(bed,"PV",981))return 1;
 SDL_FreeSurface(s);SDL_FreeSurface(cut);SDL_FreeSurface(bed);return 0;
}
static unsigned char*fontbytes;static stbtt_fontinfo font;
static int initfont(void){FILE*f=fopen("data/hd-source/DejaVuSerif.ttf","rb");if(!f)return 1;fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);fontbytes=malloc(n);if(!fontbytes){fclose(f);return 1;}int ok=fread(fontbytes,1,n,f)==(size_t)n&&stbtt_InitFont(&font,fontbytes,0);fclose(f);return !ok;}
static SDL_Surface*text(int w,int h,const char*message,int center,Uint32 rgb){
 SDL_Surface*out=canvas(w,h);int lines=1;double maxwidth=0,width=0;
 for(const unsigned char*p=(const unsigned char*)message;;p++){
  if(*p=='\n'||!*p){if(width>maxwidth)maxwidth=width;width=0;if(!*p)break;lines++;}
  else{int advance;stbtt_GetCodepointHMetrics(&font,*p,&advance,NULL);width+=advance;}
 }
 int asc,desc,gap;stbtt_GetFontVMetrics(&font,&asc,&desc,&gap);
 double scale=fmin((double)(h-8)/(lines*(asc-desc+gap)),(double)(w-8)/fmax(1,maxwidth)),lineheight=(asc-desc+gap)*scale;
 double top=(h-lines*lineheight)/2,baseline=top+asc*scale;
 const unsigned char*start=(const unsigned char*)message;
 for(int line=0;line<lines;line++){
  const unsigned char*end=start;double width=0;while(*end&&*end!='\n'){int adv;stbtt_GetCodepointHMetrics(&font,*end++,&adv,NULL);width+=adv*scale;}
  double x=center?(w-width)/2:4;
  for(const unsigned char*p=start;p<end;p++){
   int gw,gh,xoff,yoff,adv;unsigned char*a=stbtt_GetCodepointBitmap(&font,scale,scale,*p,&gw,&gh,&xoff,&yoff);
   for(int yy=0;yy<gh;yy++)for(int xx=0;xx<gw;xx++){
    int dx=(int)x+xoff+xx,dy=(int)baseline+yoff+yy;if(dx>=0&&dx<w&&dy>=0&&dy<h&&a[yy*gw+xx])put(out,dx,dy,((Uint32)a[yy*gw+xx]<<24)|(rgb&0xffffff));
   }stbtt_FreeBitmap(a,NULL);stbtt_GetCodepointHMetrics(&font,*p,&adv,NULL);x+=adv*scale;
  }baseline+=lineheight;start=*end?end+1:end;
 }return out;
}
static int titles(void){
 const char*messages[]={
 "In the Sultan's absence,\nthe Grand Vizier JAFFAR\nrules with an iron fist of\ntyranny. Only one obstacle\nremains between Jaffar and\nthe throne: the Sultan's\nbeautiful young daughter...",
 "Marry Jaffar... or die\nwithin the hour. All\nthe Princess's hopes now\nrest on the brave youth she\nloves. Little does she know\nthat he is already a prisoner\nin Jaffar's dungeons...",
 "The tyrant Jaffar lies dead, his\npower shattered. Throughout the\nland, the people of\nPersia hail their Princess... and the\nbrave youth who saved her from the\nforces of darkness. No longer a\nstranger, he shall from this day forth\nbe known as... PRINCE OF PERSIA.",
 "ORIGINAL GAME - (C) 1990 Jordan Mechner\nCreated and directed by Jordan Mechner\nDOS programming: Lance Groody\nGraphics: Leila Bronstein, Avril Harrison,\nJames St. Louis\nSound: Tom Rettig | Music: Francis Mechner\nProducer: Brian Eheler | Publisher: Broderbund Software\n\nTHIS UNOFFICIAL LINUX / HD FORK\nInspired by the original Prince of Persia\nBased on SDLPoP by David Nagy and contributors\nAI-assisted development and artwork",
 "ORIGINAL GAME PUBLISHER\nBroderbund Software","ORIGINAL GAME BY\nJordan Mechner","INSPIRED BY THE ORIGINAL GAME","Unofficial Linux / HD fork - AI-assisted"};
 const int ids[]={42,43,44,45,52,53,54,55};
 if(initfont())return 1;
 for(int k=0;k<8;k++){
  char path[128];snprintf(path,sizeof(path),"data/TITLE/res%d.png",ids[k]);SDL_Surface*n=IMG_Load(path);if(!n)return 1;
  SDL_Surface*out=text(n->w*4,n->h*4,messages[k],k>=3,k==6?0xfff1c46b:0xffefdfc7);if(!out||save(out,"TITLE",ids[k]))return 1;
  SDL_FreeSurface(out);SDL_FreeSurface(n);
 }free(fontbytes);return 0;
}
static double mask(SDL_Surface*s,double x,double y){
 int ix=(int)floor(x),iy=(int)floor(y);double sum=0;
 for(int j=0;j<2;j++)for(int i=0;i<2;i++){int xx=ix+i,yy=iy+j;if(xx<0||yy<0||xx>=s->w||yy>=s->h)continue;Uint32 p=px(s,xx,yy);double a=(p>>24)/255.0;if(!(p&0xffffff))a=0;sum+=a*(i?x-ix:1-x+ix)*(j?y-iy:1-y+iy);}
 return sum;
}
static int primitives(const char*stem,int id,int w,int h,SDL_Surface*s){
 SDL_Surface*out=canvas(w,h);int ui=!strcmp(stem,"KID")||!strcmp(stem,"GUARD")||!strcmp(stem,"FAT")||!strcmp(stem,"SKEL")||!strcmp(stem,"VIZIER")||!strcmp(stem,"SHADOW");
 int bottle=!strcmp(stem,"PRINCE")&&id>=162&&id<=165,hour=!strcmp(stem,"PV")&&id>=953&&id<=959,flow=!strcmp(stem,"PV")&&id>=960&&id<=962;
 for(int y=0;y<h;y++)for(int x=0;x<w;x++){
  double ar=0,rr=0,gg=0,bb=0;
  for(int j=0;j<4;j++)for(int i=0;i<4;i++){
   double xx=x+(i+.5)/4,yy=y+(j+.5)/4,u=xx/w,v=yy/h,a=0,r=0,g=0,b=0;
   if(hour){
    double cx=fabs(u-.5),cy=fabs(v-.5),rim=.08+.63*pow(cy/.38,1.6);
    int wood=v<.08||v>.92||((u<.13||u>.87)&&v>.08&&v<.92);
    if(wood){a=1;r=145+75*sin(u*3.14159);g=88+66*sin(u*3.14159);b=36;}
    else if(v>.12&&v<.88&&cx<rim*.46){
     a=.28;r=150;g=211;b=230;
     double phase=(id-953)/6.0;
     if((v<.5&&v>.15+phase*.31)||(v>.88-phase*.31)){a=1;r=240;g=200;b=117;}
     if(fabs(cx-rim*.46)<.024){a=.9;r=220;g=238;b=240;}
    }
   }else if(flow){if(fabs(u-.5)<.2&&fmod(yy+id*3,9)<6){a=1;r=244;g=210;b=132;}}
   else if(bottle){
    double dx=(u-.5)/.41,dy=(v-.63)/.32;
    if(dx*dx+dy*dy<1||(fabs(u-.5)<.15&&v>.1&&v<.45)){
     a=1;double shine=exp(-pow((u-.32)/.09,2));r=168+65*shine;g=104+98*shine;b=52+120*shine;
     if(v>.47&&dx*dx+dy*dy<.82){r=154+70*shine;g=30+80*shine;b=60+80*shine;}
    }
   }else if(ui&&w==24&&h==20){
    double X=(u-.5)*2.7,Y=(.48-v)*2.7,z=X*X+Y*Y-1;
    double ix=X/.72,iy=Y/.72,inside=ix*ix+iy*iy-1;
    int empty=!strcmp(stem,"KID")&&id==618;
    if(z*z*z-X*X*Y*Y*Y<=0&&(!empty||inside*inside*inside-ix*ix*iy*iy*iy>0)){a=1;r=170+70*(1-v);g=25+42*exp(-pow((u-.32)*8,2));b=43;}
   }else{
    a=mask(s,xx/4-.5,yy/4-.5)>.45;
    if(a){
     double shine=.5+.5*sin(xx*.09+yy*.017),grain=sin(xx*2.7+yy*1.3)*3;
     int mirror=!strcmp(stem,"VDUNGEON")&&id>=311&&id<=313;
     r=mirror?90+110*shine:90+125*shine+grain;g=mirror?150+80*shine:105+125*shine+grain;b=mirror?190+60*shine:120+120*shine+grain;
     if(!strcmp(stem,"VPALACE")&&id<300){r=65+40*shine;g=105+80*shine;b=150+80*shine;}
     if(ui&&w>24){r=176+65*shine;g=25+20*shine;b=35+35*shine;}
     if(!strcmp(stem,"PRINCE")&&id>=166){r=240;g=205+40*shine;b=130+90*shine;}
    }
   }
   ar+=a;rr+=a*r;gg+=a*g;bb+=a*b;
  }
  if(ar)put(out,x,y,((Uint32)(ar*255/16+.5)<<24)|((Uint32)(rr/ar)<<16)|((Uint32)(gg/ar)<<8)|(Uint32)(bb/ar));
 }
 int result=save(out,stem,id);SDL_FreeSurface(out);return result;
}
static int painted_props(void){
 const struct {const char*file,*stem;int first,count,cols,rows;} groups[]={
  {"hourglass-sheet","PV",953,7,4,2},{"items-sheet","PRINCE",160,6,3,2}
 };
 FILE*report=fopen("data/hd/painted-props-manifest.txt","w");if(!report)return 1;
 for(unsigned g=0;g<2;g++){
  char path[256];snprintf(path,sizeof(path),"data/hd-source/%s.png",groups[g].file);SDL_Surface*s=rgba(path,0);if(!s)return 1;
  for(int k=0;k<groups[g].count;k++){
   int x0=(k%groups[g].cols)*s->w/groups[g].cols,x1=(k%groups[g].cols+1)*s->w/groups[g].cols;
   int y0=(k/groups[g].cols)*s->h/groups[g].rows,y1=(k/groups[g].cols+1)*s->h/groups[g].rows;
   int l=x1,t=y1,r=x0-1,b=y0-1;
   for(int y=y0;y<y1;y++)for(int x=x0;x<x1;x++)if((px(s,x,y)>>24)>=192){if(x<l)l=x;if(x>r)r=x;if(y<t)t=y;if(y>b)b=y;}
   if(r<l||b<t){fprintf(stderr,"Missing painted prop %s %d\n",groups[g].stem,groups[g].first+k);return 1;}
   SDL_Surface*cut=canvas(r-l+1,b-t+1);SDL_Rect box={l,t,r-l+1,b-t+1};SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_NONE);SDL_BlitSurface(s,&box,cut,NULL);
   int id=groups[g].first+k;snprintf(path,sizeof(path),"data/%s/res%d.png",groups[g].stem,id);SDL_Surface*n=rgba(path,1);if(!n)return 1;
   int nl=n->w,nt=n->h,nr=-1,nb=-1;
   for(int y=0;y<n->h;y++)for(int x=0;x<n->w;x++)if((px(n,x,y)>>24)>128&&(px(n,x,y)&0xffffff)){if(x<nl)nl=x;if(x>nr)nr=x;if(y<nt)nt=y;if(y>nb)nb=y;}
   if(nr<nl||nb<nt)return 1;
   double scale=fmin(((nr-nl+1)*4-2.0)/cut->w,((nb-nt+1)*4-2.0)/cut->h);
   SDL_Surface*fit=resize(cut,fmax(1,(int)(cut->w*scale)),fmax(1,(int)(cut->h*scale))),*out=canvas(n->w*4,n->h*4);if(!fit||!out)return 1;
   SDL_Rect dest={nl*4+((nr-nl+1)*4-fit->w)/2,nt*4+((nb-nt+1)*4-fit->h)/2,0,0};SDL_SetSurfaceBlendMode(fit,SDL_BLENDMODE_NONE);SDL_BlitSurface(fit,NULL,out,&dest);
   if(save(out,groups[g].stem,id))return 1;
   fprintf(report,"%s %d sheet=%s cell=%d target=%dx%d native-ink=%d,%d..%d,%d\n",groups[g].stem,id,groups[g].file,k,out->w,out->h,nl,nt,nr,nb);
   SDL_FreeSurface(n);SDL_FreeSurface(out);SDL_FreeSurface(cut);SDL_FreeSurface(fit);
  }SDL_FreeSurface(s);
 }
 fclose(report);return 0;
}
int main(void){
 if(scenes()||titles())return 1;
 FILE*f=fopen("tools/hd_remaining.tsv","r");if(!f)return 1;char stem[64];int id,w,h,index,count=0;
 while(fscanf(f,"%63s%d%d%d%d",stem,&id,&w,&h,&index)==5){
  /* Moving jaws and their blood masks have a dedicated material packer. */
  if((!strcmp(stem,"VDUNGEON")||!strcmp(stem,"VPALACE"))&&id>=301&&id<=323)continue;
  if(!strcmp(stem,"TITLE")||(!strcmp(stem,"PV")&&(id==951||id==952||id==981)))continue;
  char path[256];snprintf(path,sizeof(path),"data/%s/res%d.png",stem,id);SDL_Surface*s=rgba(path,1);if(!s)return 1;
  if(primitives(stem,id,w*4,h*4,s))return 1;
  SDL_FreeSurface(s);count++;
 }
 fclose(f);if(painted_props())return 1;
 printf("HD finish: 13 scene/title assets + 13 painted props + %d precision props/effects; outline font at runtime\n",count-13);return 0;
}
