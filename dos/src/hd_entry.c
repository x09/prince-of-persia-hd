/* DOS game and optional art workbench share an explicit entry dispatcher. */
#include "hd_preview.h"
#include "hd_game.h"
#include <string.h>
int pop_dos_main(int argc,char **argv);
int main(int argc,char **argv) {
 for(int i=1;i<argc;++i) {
  if(!strcmp(argv[i],"--hd-check"))return pop_hd_assets_check();
  if(!strcmp(argv[i],"--hd-preview"))return pop_hd_preview(argc,argv);
 }
 const char *name=strrchr(argv[0],'/');name=name?name+1:argv[0];
 int hd=!strcmp(name,"pophd") || !strcmp(name,"PoPHD");
 for(int i=1;i<argc;++i){if(!strcmp(argv[i],"--hd"))hd=1;if(!strcmp(argv[i],"--dos"))hd=0;}
 if(hd)pop_hd_enable(1);
 return pop_dos_main(argc,argv);
}
