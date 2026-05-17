  10 #define ADDR_START 0x100
  20 #define OBJ_NUM 32
  30 #define TYPE_NONE 0
  40 #define TYPE_PLAYER 1
  50 #define TYPE_POST 2
  60 #define TYPE_SHOT 3
  70 #define TYPE_BULT 4
  80 #define TYPE_ENM1 5
  90 #define TYPE_ENM2 6
 100 #define TYPE_ENM3 7
 110 #define TYPE_ENM4 8
 120 unsigned long next = 1;
 130 void load();
 140 void title();
 150 void game();
 160 int over();
 170 char** get_spr_data();
 180 float spr[OBJ_NUM][3];
 190 unsigned char type[OBJ_NUM];
 200 unsigned char step[OBJ_NUM];
 210 unsigned char flag[OBJ_NUM];
 220 float delX[OBJ_NUM], delY[OBJ_NUM];
 230 double px,py,pa;
 240 float sx,sy,dx,dy;
 250 int score;
 260 main(){
 270 load();
 280 title();
 290 do{
 300 game();
 310 if(over() != 0) break;
 320 }while(1);
 330 }
 340 void wait_v(t){
 350 int i=0;
 360 for(i=0;i<t;i++);
 370 }
 380 int my_rand(void) {
 390 next = next*1103515245+12345;
 400 return (unsigned int)(next/65536)%32768;
 410 }
 420 int get_random(int max) {
 430 if (max <= 0) return 0;
 440 return my_rand() % max;
 450 }
 460 unsigned char hex2val(char c){
 470 if (c >='0' && c <='9') return c-'0';
 480 if (c >='A' && c <='F') return c-'A'+10;
 490 if (c >='a' && c <='f') return c-'a'+10;
 500 return 0;
 510 }
 520 unsigned char h2b(const char *s) {
 530 return (hex2val(s[0]) << 4) | hex2val(s[1]);
 540 }
 550 void load(){
 560 unsigned int ptr=ADDR_START;
 570 int i,j,k,key,d=0,e=35,f=0;
 580 char bar[24],mark[4]={'/','-','|','.'};
 590 char** spr_data = get_spr_data();
 600 clrscr();
 610 printf("\n    1:Load  2:Skip\n\n Initial load required");
 620 do{
 630 next++;
 640 key=getch();
 650 if(key =='1') break;
 660 if(key =='2') return;
 670 }while(1);
 680 clrscr();
 690 printf("WRITING MACHINE CODE..\n");
 700 printf("WAIT ABOUT 10 SECONDS\n");
 710 printf("KEEP POWER ON!\n");
 720 for(i=0;spr_data[i] != NULL;i++){
 730 for (j=0; spr_data[i][j] != '\0';j+=2){
 740 poke(ptr++, h2b(&spr_data[i][j]));
 750 }
 760 d++; f=22*d/e; bar[0]=NULL;
 770 gotoxy(0,3); printf("[%d%%]\n", 100*d/e);
 780 strcat(bar, "[");
 790 for(k=0;k<f;k++) strcat(bar, "*");
 800 for(k=0;k<22-f;k++) strcat(bar, "-");
 810 strcat(bar, "]");
 820 printf(bar);
 830 gotoxy(22,0); printf("%c", mark[d%4]);
 840 }
 850 }
 860 void title(){
 870 int key;
 880 clrscr();
 890 printf("\n\n     SILVER GLIDE");
 900 printf("\n\n     press 1 start");
 910 do{
 920 next++;
 930 key=getch();
 940 if(key =='1') break;
 950 }while(1);
 960 }
 970 int getObjNo(){
 980 int i;
 990 for(i=1;i<32;i++){
1000 if(type[i] == TYPE_NONE){
1010 step[i]=0; flag[i]=0;
1020 return(i);
1030 }
1040 }
1050 return(-1);
1060 }
1070 void shotHit(int i){
1080 int j;
1090 for(j=1;j<OBJ_NUM;j++){
1100 if(type[j] == TYPE_POST || (type[j] >= TYPE_ENM1 && step[j] < 90)){
1110 dx=spr[j][1]; dy=spr[j][2];
1120 if(dx-6 < sx && dx+6 > sx && dy-8 < sy && dy+8 > sy){
1130 if(type[j] >= TYPE_ENM1){
1140 score+=100;
1150 step[j]=90;
1160 }
1170 spr[i][1]=px+4; spr[i][2]=py;
1180 }
1190 }
1200 }
1210 }
1220 void post(int y){
1230 int i=getObjNo();
1240 if(i > 0){
1250 type[i]=TYPE_POST;
1260 spr[i][0]=1; spr[i][1]=144; spr[i][2]=y;
1270 }
1280 }
1290 void bullet(int way){
1300 int i=getObjNo();
1310 if(i > 0){
1320 type[i]=TYPE_BULT;
1330 spr[i][0]=5; spr[i][1]=sx; spr[i][2]=sy;
1340 switch(way){
1350 case 0:
1360 delX[i]=-2.5; delY[i]=(py-sy)/60.0;
1370 break;
1380 case 1:
1390 delX[i]=-2.5; delY[i]=0;
1400 break;
1410 case 2:
1420 delX[i]=-2.5; delY[i]=1.25;
1430 break;
1440 case 3:
1450 delX[i]=-2.5; delY[i]=-1.25;
1460 break;
1470 }
1480 }
1490 }
1500 void game(){
1510 int i,j,r,y,loop=1,enmInt=20,plyHit=0;
1520 float enmRnd=28;
1530 unsigned char keybit=0;
1540 score=0;
1550 clrscr();
1560 printf("\nSILVER");
1570 printf("\n GLIDE");
1580 line(47, 0, 47, 47, 0, 0xffff, 0);
1590 px=40.0; py=28.0; pa=0.0;
1600 for(i=0;i<OBJ_NUM;i++){
1610 type[i]=TYPE_NONE; step[i]=0;
1620 }
1630 type[0]=TYPE_PLAYER; spr[0][0]=2; spr[0][1]=px; spr[0][2]=py;
1640 while(loop==1){
1650 plyHit=0;
1660 for(i=0;i<OBJ_NUM;i++){
1670 if(type[i] > TYPE_NONE){
1680 switch(type[i]){
1690 case TYPE_PLAYER:
1700 switch(step[i]){
1710 case 0:
1720 px+=1.5;
1730 if(px >= 60){
1740 j=getObjNo();
1750 type[j]=TYPE_SHOT; delX[j]=5;
1760 spr[j][0]=4; spr[j][1]=px+4; spr[j][2]=py;
1770 step[i]++;
1780 }
1790 break;
1800 case 1:
1810 outport(0x11, 0x40);
1820 keybit=inport(0x10);
1830 if((keybit & 0x38) != 0) pa-=0.4;
1840 outport(0x11, 0x20);
1850 keybit=inport(0x10);
1860 if((keybit & 0x38) != 0) pa+=0.4;
1870 py+=pa;
1880 if(py < (-1) || py > 58) plyHit=1;
1890 break;
1900 }
1910 spr[i][0]=2; spr[i][1]=px; spr[i][2]=py;
1920 break;
1930 case TYPE_POST:
1940 spr[i][0]=1; spr[i][1]-=1.0f; if(spr[i][1] <= 40) type[i]=TYPE_NONE;
1950 dx=spr[i][1]; dy=spr[i][2];
1960 if(dx-6 < px && dx+6 > px && dy-6 < py && dy+6 > py) plyHit=1;
1970 break;
1980 case TYPE_SHOT:
1990 spr[i][1]+=delX[i];
2000 sx=spr[i][1]; sy=spr[i][2];
2010 if(sx < 144){
2020 shotHit(i);
2030 }else{
2040 spr[i][1]=px+4; spr[i][2]=py;
2050 }
2060 break;
2070 case TYPE_BULT:
2080 spr[i][1]+=delX[i]; spr[i][2]+=delY[i];
2090 dx=spr[i][1]; dy=spr[i][2];
2100 if(dx >= 40 && dy >= 0 && dy < 56){
2110 if(dx-2 < px && dx+2 > px && dy-2 < py && dy+2 > py){
2120 plyHit=1;
2130 type[i]=TYPE_NONE;
2140 }
2150 }else{
2160 type[i]=TYPE_NONE;
2170 }
2180 break;
2190 case TYPE_ENM1:
2200 spr[i][1]+=delX[i]; spr[i][2]+=delY[i];
2210 sx=spr[i][1]; sy=spr[i][2];
2220 if(step[i] < 90){
2230 if(sx-4 < px && sx+4 > px && sy-4 < py && sy+4 > py){
2240 spr[i][0]=3; plyHit=1;
2250 }
2260 }
2270 switch(step[i]){
2280 case 0:
2290 if(sx < 72){
2300 delY[i]= -delY[i];
2310 step[i]++;
2320 }
2330 break;
2340 case 1:
2350 delX[i]+=0.2;
2360 if(delX[i] >= 2.0){
2370 delY[i]= -delY[i];
2380 step[i]++;
2390 }
2400 break;
2410 case 2:
2420 if(sx >= 144) type[i]=TYPE_NONE;
2430 break;
2440 case 90:
2450 spr[i][0]=3; step[i]++;
2460 break;
2470 case 91:
2480 type[i]=TYPE_NONE;
2490 break;
2500 }
2510 break;
2520 case TYPE_ENM2:
2530 spr[i][1]+=delX[i];
2540 sx=spr[i][1]; sy=spr[i][2];
2550 switch(step[i]){
2560 case 0:
2570 if(sx >= 40){
2580 dx=spr[i][1]; dy=spr[i][2];
2590 if(dx-4 < px && dx+4 > px && dy-4 < py && dy+4 > py){
2600 spr[i][0]=3; plyHit=1;
2610 }
2620 }else{
2630 type[i]=TYPE_NONE;
2640 }
2650 break;
2660 case 90:
2670 spr[i][0]=3; step[i]++;
2680 break;
2690 case 91:
2700 type[i]=TYPE_NONE;
2710 break;
2720 }
2730 break;
2740 case TYPE_ENM3:
2750 spr[i][1]+=delX[i];
2760 sx=spr[i][1]; sy=spr[i][2];
2770 switch(step[i]){
2780 case 0:
2790 if(sx >= 40){
2800 dx=spr[i][1]; dy=spr[i][2];
2810 if(dx-4 < px && dx+4 > px && dy-4 < py && dy+4 > py){
2820 spr[i][0]=3; plyHit=1;
2830 }
2840 flag[i]++;
2850 if(flag[i] > 20){
2860 flag[i]=0;
2870 bullet(1); bullet(2); bullet(3);
2880 }
2890 }else{
2900 type[i]=TYPE_NONE;
2910 }
2920 break;
2930 case 90:
2940 spr[i][0]=3; step[i]++;
2950 break;
2960 case 91:
2970 type[i]=TYPE_NONE;
2980 break;
2990 }
3000 break;
3010 case TYPE_ENM4:
3020 spr[i][1]+=delX[i];
3030 sx=spr[i][1]; sy=spr[i][2];
3040 switch(step[i]){
3050 case 0:
3060 if(sx >= 40){
3070 dx=spr[i][1]; dy=spr[i][2];
3080 if(dx-4 < px && dx+4 > px && dy-4 < py && dy+4 > py){
3090 spr[i][0]=3; plyHit=1;
3100 }
3110 if(flag[i] == 0 && sx < 120){
3120 flag[i]=1;
3130 bullet(0);
3140 }
3150 }else{
3160 type[i]=TYPE_NONE;
3170 }
3180 break;
3190 case 90:
3200 spr[i][0]=3; step[i]++;
3210 break;
3220 case 91:
3230 type[i]=TYPE_NONE;
3240 break;
3250 }
3260 break;
3270 }
3280 }else{
3290 spr[i][0]=0;
3300 }
3310 }
3320 enmInt--;
3330 if(enmInt <= 0){
3340 enmInt=enmRnd+get_random(enmRnd);
3350 if(enmRnd > 11) enmRnd-=0.5;
3360 r=get_random(120);
3370 if(r < 10){
3380 y=7+get_random(40);
3390 post(y);
3400 }else if(r < 20){
3410 y=7+get_random(32);
3420 post(y); y+=8; post(y);
3430 }else if(r < 50){
3440 y=10+get_random(38);
3450 i=getObjNo();
3460 if(i > 0){
3470 type[i]=TYPE_ENM1; step[i]=0; delX[i]=-2.8; delY[i]=-0.5;
3480 spr[i][0]=6; spr[i][1]=144; spr[i][2]=y;
3490 }
3500 }else if(r < 80){
3510 y=7+get_random(40);
3520 i=getObjNo();
3530 if(i > 0){
3540 type[i]=TYPE_ENM2; step[i]=0; delX[i]=-1.5; delY[i]=0;
3550 spr[i][0]=7; spr[i][1]=144; spr[i][2]=y;
3560 }
3570 }else if(r < 90){
3580 y=7+get_random(40);
3590 i=getObjNo();
3600 if(i > 0){
3610 type[i]=TYPE_ENM3; step[i]=0; delX[i]=-1.0; delY[i]=0;
3620 spr[i][0]=8; spr[i][1]=144; spr[i][2]=y;
3630 }
3640 }else{
3650 y=7+get_random(40);
3660 i=getObjNo();
3670 if(i > 0){
3680 type[i]=TYPE_ENM4; step[i]=0; delX[i]=-1.5; delY[i]=0;
3690 spr[i][0]=7; spr[i][1]=144; spr[i][2]=y;
3700 }
3710 }
3720 }
3730 if(plyHit != 0){
3740 spr[0][0]=3; loop=0;
3750 }
3760 call(ADDR_START, (void*)spr);
3770 if(score < 1000000) score++;
3780 gotoxy(0, 4); printf("%06d", score);
3790 }
3800 wait_v(2000);
3810 return;
3820 }
3830 int over(){
3840 int key;
3850 clrscr();
3860 printf("\n       Game Over");
3870 printf("\n        %06d",score);
3880 printf("\n\n      2:retry 3:end");
3890 do{
3900 key=getch();
3910 if(key =='2') return(0);
3920 if(key =='3') break;
3930 }while(1);
3940 clrscr();
3950 return(1);
3960 }
3970 const char *spr_data_internal[] = {
3980 "CD0A01CD4601CD1D01C9E5211A030100",
3990 "05160072230B78B1C21301E1C9211A03",
4000 "01D8000916B07AD3403E00D3403E13D3",
4010 "403E60477E23D34110FA01400009147A",
4020 "FEB6C22601C9062022D302DD2AD302C5",
4030 "CD5802ED53CD02110400DD19CD5802ED",
4040 "53CF02110400DD19CD5802ED53D10211",
4050 "0400DD19ED5BCD027BFE00CA98013D5F",
4060 "1600CB23CB12CB23CB12CB23CB1221DA",
4070 "021922CD02CD9C01C110B4C92ACF0211",
4080 "080019CB7CC257021160FF19CB7CCA57",
4090 "0211A0001922D3022AD10211090019CB",
4100 "7CC2570211C0FF19CB7CCA5702113700",
4110 "1922D502CB2CCB1DCB2CCB1DCB2CCB1D",
4120 "7D2AD302FE00CAF0014711A0001910FD",
4130 "ED5BD5027BE607FE07C21302111A0319",
4140 "11A00019ED5BCD0206081A4EB1771323",
4150 "10F8C93C32D702111A0319FD2ACD020E",
4160 "00FD56001E00FD233AD70247CB22CB13",
4170 "10FA7A32D8027B32D9023AD802577EB2",
4180 "7711A000193AD902577EB2771161FF19",
4190 "0C79FE08C22102C9DD7E01E6F0FE20CA",
4200 "9102FE10CA7602DD7E020F0F0F0FE60F",
4210 "16005FC3BB02DD7E020F0F0F0FE60F4F",
4220 "878781874FDD7E02E60F8116005FC3BB",
4230 "02DD7E030F0F0F0FE60F6FDD7E02E60F",
4240 "4F878781878526006FDD7E020F0F0F0F",
4250 "E60F471164001910FD545DDD7E01E608",
4260 "FE00CACC027A2F577B2F5F13C9000000",
4270 "00000000000000000000FF81FFFF81FF",
4280 "81FF577E7C5C4C0C0C08539672595C8C",
4290 "2E09000081C37E3C00000018245A5A24",
4300 "18008C52767E3C6E614060D2D3F3F37F",
4310 "7E3840C8ECF6F7FFFE78000000000000",
4320 NULL
4330 };
4340 char** get_spr_data(){ return spr_data_internal; }
