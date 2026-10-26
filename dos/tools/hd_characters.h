/* Resource ranges contain native blank placeholders as well as figures. */
typedef struct {const char*name,*stem;int first,count,cols,rows;} HDCharacter;
static const HDCharacter hd_characters[]={
 {"skel","SKEL",753,26,6,5},
 {"fat","FAT",753,32,6,6},
 {"vizier","VIZIER",753,32,6,6},
 {"shadow","SHADOW",753,23,6,5},
 {"shadow-fall","SHADOW",776,1,1,1},
 {"shadow","SHADOW",777,6,6,5},
 {"princess-story","PV",801,17,5,4},
 {"jaffar-story","PV",851,32,6,6},
 {"jaffar-end","PV",883,6,3,2},
 {"princess-cut","PV",901,30,6,5},
};
#define HD_CHARACTER_GROUPS (sizeof(hd_characters)/sizeof(hd_characters[0]))
