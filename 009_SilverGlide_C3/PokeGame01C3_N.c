  10 #define ADDR_START 0x100
  20 #define ADDR_SPR_PRM 0x104
  30 #define ADDR_SPR_POS 0x106
  40 #define OBJ_NUM 32
  50 #define TYPE_NONE 0
  60 #define TYPE_PLAYER 1
  70 #define TYPE_SHOT 2
  80 #define TYPE_BULT 3
  90 #define TYPE_ENM1 4
 100 #define TYPE_ENM2 5
 110 #define TYPE_ENM3 6
 120 #define TYPE_ENM4 7
 130 #define TYPE_POST 8
 140 #define TYPE_POST2 9
 150 #define TYPE_BOSS 10
 160 unsigned long next = 1;
 170 void load();
 180 void title();
 190 void game();
 200 int over();
 210 char** get_spr_data();
 220 unsigned char sprPrm[OBJ_NUM][3];
 230 float sprPos[OBJ_NUM][2];
 240 unsigned char type[OBJ_NUM];
 250 unsigned char step[OBJ_NUM];
 260 unsigned char flag[OBJ_NUM];
 270 float delX[OBJ_NUM], delY[OBJ_NUM];
 280 double px,py,pa;
 290 float sx,sy,ex,ey;
 300 int score;
 310 int plyHit;
 320 int bossHP;
 330 short hl=0;
 340 void pokeAdr(unsigned int mem, void* adr){
 350 unsigned short a=(unsigned short)adr;
 360 poke(mem+0, a&0xff);
 370 poke(mem+1, (a>>8)&0xff);
 380 }
 390 main(){
 400 load();
 410 pokeAdr(ADDR_SPR_PRM, sprPrm);
 420 pokeAdr(ADDR_SPR_POS, sprPos);
 430 title();
 440 do{
 450 game();
 460 if(over() != 0) break;
 470 }while(1);
 480 }
 490 void wait_v(t){
 500 int i=0;
 510 for(i=0;i<t;i++);
 520 }
 530 int my_rand(void) {
 540 next = next*1103515245+12345;
 550 return (unsigned int)(next/65536)%32768;
 560 }
 570 int get_random(int max) {
 580 if (max <= 0) return 0;
 590 return my_rand() % max;
 600 }
 610 unsigned char hex2val(char c){
 620 if (c >='0' && c <='9') return c-'0';
 630 if (c >='A' && c <='F') return c-'A'+10;
 640 if (c >='a' && c <='f') return c-'a'+10;
 650 return 0;
 660 }
 670 unsigned char h2b(const char *s) {
 680 return (hex2val(s[0]) << 4) | hex2val(s[1]);
 690 }
 700 void load(){
 710 unsigned int ptr=ADDR_START;
 720 int i,j,k,key,d=0,e=43,f=0;
 730 char bar[24],mark[4]={'/','-','|','.'};
 740 char** spr_data = get_spr_data();
 750 clrscr();
 760 printf("\n    1:Load  2:Skip\n\n Initial load required");
 770 do{
 780 next++;
 790 key=getch();
 800 if(key =='1') break;
 810 if(key =='2') return;
 820 }while(1);
 830 clrscr();
 840 printf("WRITING MACHINE CODE..\n");
 850 printf("WAIT ABOUT 10 SECONDS\n");
 860 printf("KEEP POWER ON!\n");
 870 for(i=0;spr_data[i] != NULL;i++){
 880 for (j=0; spr_data[i][j] != '\0';j+=2){
 890 poke(ptr++, h2b(&spr_data[i][j]));
 900 }
 910 d++; f=22*d/e; bar[0]=NULL;
 920 gotoxy(0,3); printf("[%d%%]\n", 100*d/e);
 930 strcat(bar, "[");
 940 for(k=0;k<f;k++) strcat(bar, "*");
 950 for(k=0;k<22-f;k++) strcat(bar, "-");
 960 strcat(bar, "]");
 970 printf(bar);
 980 gotoxy(22,0); printf("%c", mark[d%4]);
 990 }
1000 }
1010 void title(){
1020 int key;
1030 clrscr();
1040 printf("\n\n     SILVER GLIDE");
1050 printf("\n\n     press 1 start");
1060 do{
1070 next++;
1080 key=getch();
1090 if(key =='1') break;
1100 }while(1);
1110 }
1120 int getObjNo(){
1130 int i;
1140 for(i=1;i<32;i++){
1150 if(type[i] == TYPE_NONE){
1160 step[i]=0; flag[i]=0;
1170 return(i);
1180 }
1190 }
1200 return(-1);
1210 }
1220 void shotHit(int i){
1230 int j,w,h;
1240 for(j=1;j<OBJ_NUM;j++){
1250 if(type[j] == TYPE_POST || (type[j] >= TYPE_ENM1 && step[j] < 90)){
1260 ex=sprPos[j][0]; ey=sprPos[j][1];
1270 w=6+(sprPrm[j][1]-1)*8;
1280 h=sprPrm[j][2]*8;
1290 if(ex-6 < sx && ex+w > sx && ey-8 < sy && ey+h > sy){
1300 if(type[j] < TYPE_POST){
1310 score+=100;
1320 step[j]=90;
1330 }else if(type[j]==TYPE_BOSS){
1340 bossHP--;
1350 if(bossHP <= 0){
1360 score+=3000;
1370 step[j]=90;
1380 }
1390 }
1400 sprPos[i][0]=px+4; sprPos[i][1]=py;
1410 }
1420 }
1430 }
1440 }
1450 int enmHit(int i){
1460 ex=sprPos[i][0]; ey=sprPos[i][1];
1470 if(ex-4 < px && ex+4 > px && ey-4 < py && ey+4 > py){
1480 sprPrm[i][0]=2; step[i]=90; plyHit=1;
1490 }
1500 }
1510 int makeEnm(int t,int n, float y, float dx, float dy){
1520 int i=getObjNo();
1530 if(i > 0){
1540 type[i]=t; step[i]=0;
1550 sprPrm[i][0]=n; sprPrm[i][1]=1; sprPrm[i][2]=1;
1560 sprPos[i][0]=144; sprPos[i][1]=y;
1570 delX[i]=dx; delY[i]=dy;
1580 }
1590 return i;
1600 }
1610 void bullet(int way){
1620 int i=getObjNo();
1630 if(i > 0){
1640 type[i]=TYPE_BULT;
1650 sprPrm[i][0]=4; sprPrm[i][1]=1; sprPrm[i][2]=1;
1660 sprPos[i][0]=ex; sprPos[i][1]=ey;
1670 switch(way){
1680 case 0:
1690 delX[i]=-2.5; delY[i]=(py-ey)/60.0;
1700 break;
1710 case 1:
1720 delX[i]=-2.5; delY[i]=0;
1730 break;
1740 case 2:
1750 delX[i]=-2.5; delY[i]=1.25;
1760 break;
1770 case 3:
1780 delX[i]=-2.5; delY[i]=-1.25;
1790 break;
1800 }
1810 }
1820 }
1830 void game(){
1840 int i,j,r,y,loop=1,enmInt=20,enmCnt=20,bossF=0;
1850 float enmRnd=28;
1860 unsigned char keybit=0;
1870 score=0;
1880 plyHit=0;
1890 clrscr();
1900 printf("\nSILVER");
1910 printf("\n GLIDE");
1920 line(47, 0, 47, 47, 0, 0xffff, 0);
1930 px=40.0; py=28.0; pa=0.0;
1940 for(i=0;i<OBJ_NUM;i++){
1950 type[i]=TYPE_NONE; step[i]=0;
1960 sprPrm[i][0]=0; sprPrm[i][1]=1; sprPrm[i][2]=1;
1970 }
1980 type[0]=TYPE_PLAYER; sprPrm[0][0]=1; sprPos[0][0]=px; sprPos[0][1]=py;
1990 while(loop==1){
2000 plyHit=0;
2010 for(i=0;i<OBJ_NUM;i++){
2020 if(type[i] > TYPE_NONE){
2030 if(type[i] != TYPE_PLAYER){
2040 sprPos[i][0]+=delX[i]; sprPos[i][1]+=delY[i];
2050 }
2060 switch(type[i]){
2070 case TYPE_PLAYER:
2080 switch(step[i]){
2090 case 0:
2100 px+=1.5;
2110 if(px >= 60){
2120 j=getObjNo();
2130 type[j]=TYPE_SHOT; delX[j]=5;
2140 sprPrm[j][0]=3; sprPos[j][0]=px+4; sprPos[j][1]=py;
2150 step[i]++;
2160 }
2170 break;
2180 case 1:
2190 outport(0x11, 0x40);
2200 keybit=inport(0x10);
2210 if((keybit & 0x38) != 0){
2220 pa-=0.4; get_random(100);
2230 }
2240 outport(0x11, 0x20);
2250 keybit=inport(0x10);
2260 if((keybit & 0x38) != 0){
2270 pa+=0.4; get_random(100);
2280 }
2290 py+=pa;
2300 if(py < (-1) || py > 58) plyHit=1;
2310 break;
2320 }
2330 sprPos[i][0]=px; sprPos[i][1]=py;
2340 break;
2350 case TYPE_POST:
2360 if(sprPos[i][0] <= 40) type[i]=TYPE_NONE;
2370 ex=sprPos[i][0]; ey=sprPos[i][1];
2380 if(ex-6 < px && ex+6 > px && ey-6 < py && ey+6 > py) plyHit=1;
2390 break;
2400 case TYPE_POST2:
2410 if(sprPos[i][0] <= 40) type[i]=TYPE_NONE;
2420 ex=sprPos[i][0]; ey=sprPos[i][1];
2430 if(ex-6 < px && ex+6 > px && ey-6 < py && ey+14 > py) plyHit=1;
2440 break;
2450 case TYPE_SHOT:
2460 sx=sprPos[i][0]; sy=sprPos[i][1];
2470 if(sx < 144){
2480 shotHit(i);
2490 }else{
2500 sprPos[i][0]=px+4; sprPos[i][1]=py;
2510 }
2520 break;
2530 case TYPE_BULT:
2540 ex=sprPos[i][0]; ey=sprPos[i][1];
2550 if(ex >= 40 && ey >= 0 && ey < 56){
2560 if(ex-2 < px && ex+2 > px && ey-2 < py && ey+2 > py){
2570 plyHit=1;
2580 type[i]=TYPE_NONE;
2590 }
2600 }else{
2610 type[i]=TYPE_NONE;
2620 }
2630 break;
2640 case TYPE_ENM1:
2650 if(step[i] < 90){
2660 enmHit(i);
2670 }
2680 switch(step[i]){
2690 case 0:
2700 if(sprPos[i][0] < 72){
2710 delY[i]= -delY[i];
2720 step[i]++;
2730 }
2740 break;
2750 case 1:
2760 delX[i]+=0.2;
2770 if(delX[i] >= 2.0){
2780 delY[i]= -delY[i];
2790 step[i]++;
2800 }
2810 break;
2820 case 2:
2830 if(sprPos[i][0] >= 144) type[i]=TYPE_NONE;
2840 break;
2850 case 90:
2860 sprPrm[i][0]=2; step[i]++;
2870 break;
2880 case 91:
2890 type[i]=TYPE_NONE;
2900 break;
2910 }
2920 break;
2930 case TYPE_ENM2:
2940 switch(step[i]){
2950 case 0:
2960 if(sprPos[i][0] >= 40){
2970 enmHit(i);
2980 }else{
2990 type[i]=TYPE_NONE;
3000 }
3010 break;
3020 case 90:
3030 sprPrm[i][0]=2; step[i]++;
3040 break;
3050 case 91:
3060 type[i]=TYPE_NONE;
3070 break;
3080 }
3090 break;
3100 case TYPE_ENM3:
3110 switch(step[i]){
3120 case 0:
3130 if(sprPos[i][0] >= 40){
3140 enmHit(i);
3150 flag[i]++;
3160 if(flag[i] > 30){
3170 flag[i]=0;
3180 bullet(1); bullet(2); bullet(3);
3190 }
3200 }else{
3210 type[i]=TYPE_NONE;
3220 }
3230 break;
3240 case 90:
3250 sprPrm[i][0]=2; step[i]++;
3260 break;
3270 case 91:
3280 type[i]=TYPE_NONE;
3290 break;
3300 }
3310 break;
3320 case TYPE_ENM4:
3330 switch(step[i]){
3340 case 0:
3350 if(sprPos[i][0] >= 40){
3360 enmHit(i);
3370 if(flag[i] == 0 && sprPos[i][0] < 120){
3380 flag[i]=1;
3390 bullet(0);
3400 }
3410 }else{
3420 type[i]=TYPE_NONE;
3430 }
3440 break;
3450 case 90:
3460 sprPrm[i][0]=2; step[i]++;
3470 break;
3480 case 91:
3490 type[i]=TYPE_NONE;
3500 break;
3510 }
3520 break;
3530 case TYPE_BOSS:
3540 if(step[i] < 90){
3550 enmHit(i);
3560 }
3570 switch(step[i]){
3580 case 0:
3590 if(sprPos[i][0] < 112){
3600 delX[i]=0; delY[i]=-1.2;
3610 step[i]++;
3620 }
3630 break;
3640 case 1:
3650 delY[i]+=0.1;
3660 if(delY[i] >= 1.2){
3670 bullet(0); bullet(2); bullet(3);
3680 step[i]++;
3690 }
3700 break;
3710 case 2:
3720 delY[i]-=0.1;
3730 if(delY[i] <=-1.2){
3740 bullet(0); bullet(2); bullet(3);
3750 step[i]--;
3760 }
3770 break;
3780 case 90:
3790 delY[i]=1.0; step[i]++;
3800 case 91:
3810 if(sprPos[i][1] <= 64){
3820 j=(int)sprPos[i][1];
3830 sprPrm[i][0]=10;
3840 if(j%2 == 0) sprPrm[i][0]=0;
3850 }else{
3860 bossF=0; type[i]=TYPE_NONE;
3870 }
3880 break;
3890 }
3900 break;
3910 }
3920 }else{
3930 sprPrm[i][0]=0;
3940 }
3950 }
3960 enmInt--;
3970 if(enmInt <= 0){
3980 if(bossF == 0){
3990 enmCnt--;
4000 if(enmCnt <= 0){
4010 i=makeEnm(TYPE_BOSS, 10, 24,-1.0, 0);
4020 if(i > 0) {
4030 sprPrm[i][1]=3;
4040 sprPrm[i][2]=2;
4050 enmCnt=20;
4060 bossHP=20;
4070 bossF=1;
4080 }
4090 }
4100 }
4110 enmInt=enmRnd+get_random(enmRnd);
4120 if(enmRnd > 11) enmRnd-=0.5;
4130 r=get_random(120);
4140 if(r < 10){
4150 y=7+get_random(40);
4160 makeEnm(TYPE_POST, 8, y,-1.25, 0);
4170 }else if(r < 20){
4180 y=7+get_random(32);
4190 i=makeEnm(TYPE_POST, 8, y,-1.0, 0);
4200 if(i > 0) sprPrm[i][2]=2;
4210 }else if(r < 50){
4220 y=10+get_random(38);
4230 makeEnm(TYPE_ENM1, 5, y,-2.8,-0.5);
4240 }else if(r < 80){
4250 y=7+get_random(40);
4260 makeEnm(TYPE_ENM2, 6, y,-1.5, 0);
4270 }else if(r < 90){
4280 y=7+get_random(40);
4290 makeEnm(TYPE_ENM3, 7, y,-1.0, 0);
4300 }else{
4310 y=7+get_random(40);
4320 makeEnm(TYPE_ENM4, 6, y,-1.5, 0);
4330 i=getObjNo();
4340 }
4350 }
4360 if(plyHit != 0){
4370 sprPrm[0][0]=2; loop=0;
4380 }
4390 call(ADDR_START, &hl);
4400 if(score < 1000000) score++;
4410 gotoxy(0, 4); printf("%06d", score);
4420 }
4430 wait_v(2000);
4440 return;
4450 }
4460 int over(){
4470 int key;
4480 clrscr();
4490 printf("\n       Game Over");
4500 printf("\n        %06d",score);
4510 printf("\n\n      2:retry 3:end");
4520 do{
4530 key=getch();
4540 if(key =='2') return(0);
4550 if(key =='3') break;
4560 }while(1);
4570 clrscr();
4580 return(1);
4590 }
4600 const char *spr_data_internal[] = {
4610 "C30801CD00000000CD1201CD4C01CD23",
4620 "01C921A703010005160072230B78B1C2",
4630 "1A01C921A70301D8000916B07AD3403E",
4640 "00D3403E13D3403E60477E23D34110FA",
4650 "01400009147AFEB6C22C01C90620FD2A",
4660 "0401DD2A0601C5FD7E00321D03FD7E01",
4670 "321E03FD7E02321F03110300FD19CDA8",
4680 "02ED532503110400DD19CDA802ED5327",
4690 "03110400DD193A1D03FE00CADC013D5F",
4700 "1600CB23CB12CB23CB12CB23CB12212F",
4710 "03192223032A25032229033A1F0347C5",
4720 "3A1E0347C5CDE2011108002A23031922",
4730 "23032A250319222503C110E82A290322",
4740 "25032A270319222703C110D3C105C256",
4750 "01C9DDE5FDE52A250311080019CB7CC2",
4760 "A3021160FF19CB7CCAA30211A0001922",
4770 "2B032A270311090019CB7CC2A30211C0",
4780 "FF19CB7CCAA30211370019222D03CB2C",
4790 "CB1DCB2CCB1DCB2CCB1D7D2A2B03FE00",
4800 "CA3A024711A0001910FDED5B2D037BE6",
4810 "07FE07C25F0211A7031911A00019ED5B",
4820 "230306081A4EB177132310F8C3A3023C",
4830 "32200311A70319FD2A23030E00FD5600",
4840 "1E00FD233A200347CB22CB1310FA7A32",
4850 "21037B3222033A2103577EB27711A000",
4860 "193A2203577EB2771161FF190C79FE08",
4870 "C26D02FDE1DDE1C9DD7E01E6F0FE20CA",
4880 "E102FE10CAC602DD7E020F0F0F0FE60F",
4890 "16005FC30B03DD7E020F0F0F0FE60F4F",
4900 "878781874FDD7E02E60F8116005FC30B",
4910 "03DD7E030F0F0F0FE60F6FDD7E02E60F",
4920 "4F878781878526006FDD7E020F0F0F0F",
4930 "E60F471164001910FD545DDD7E01E608",
4940 "FE00CA1C037A2F577B2F5F13C9000000",
4950 "00000000000000000000000000000057",
4960 "7E7C5C4C0C0C08539672595C8C2E0900",
4970 "0081C37E3C00000018245A5A2418008C",
4980 "52767E3C6E614060D2D3F3F37F7E3840",
4990 "C8ECF6F7FFFE78FF81FFFF81FF81FFFF",
5000 "81FFFF81FF81FF80C040C0C0C0C0C0C0",
5010 "E0F0F8FCFCCCCCC8E0F0301080C0C003",
5020 "070F0A0F0F0F1F1F3F0F0F0F0F0F0B1D",
5030 "3F3F2F272D0F0A000000000000000000",
5040 NULL
5050 };
5060 char** get_spr_data(){ return spr_data_internal; }
