  10 #define ADDR_START 0x100
  20 struct sprites{
  30 float no;
  40 float x;
  50 float y;
  60 };
  70 unsigned long next = 1;
  80 void load();
  90 void title();
 100 int game(struct sprites*);
 110 int over(int);
 120 char** get_spr_data();
 130 main(){
 140 struct sprites p[32];
 150 int s=0;
 160 load();
 170 title();
 180 do{
 190 s=game(p);
 200 if(over(s) != 0) break;
 210 }while(1);
 220 }
 230 void wait_v(t){
 240 int i=0;
 250 for(i=0;i<t;i++);
 260 }
 270 int my_rand(void) {
 280 next = next*1103515245+12345;
 290 return (unsigned int)(next/65536)%32768;
 300 }
 310 int get_random(int max) {
 320 if (max <= 0) return 0;
 330 return my_rand() % max;
 340 }
 350 unsigned char hex2val(char c){
 360 if (c >='0' && c <='9') return c-'0';
 370 if (c >='A' && c <='F') return c-'A'+10;
 380 if (c >='a' && c <='f') return c-'a'+10;
 390 return 0;
 400 }
 410 unsigned char h2b(const char *s) {
 420 return (hex2val(s[0]) << 4) | hex2val(s[1]);
 430 }
 440 void load(){
 450 unsigned int ptr=ADDR_START;
 460 int i,j,k,key,c=0x100,d=0,e=33,f=0;
 470 char bar[24],mark[4]={'/','-','|','.'};
 480 char** spr_data = get_spr_data();
 490 clrscr();
 500 printf("\n    1:Load  2:Skip\n\n Initial load required");
 510 do{
 520 next++;
 530 key=getch();
 540 if(key =='1') break;
 550 if(key =='2') return;
 560 }while(1);
 570 clrscr();
 580 printf("WRITING MACHINE CODE..\n");
 590 printf("WAIT ABOUT 10 SECONDS\n");
 600 printf("KEEP POWER ON!\n");
 610 for(i=0;spr_data[i] != NULL;i++){
 620 for (j=0; spr_data[i][j] != '\0';j+=2){
 630 poke(ptr++, h2b(&spr_data[i][j]));
 640 }
 650 d++; f=22*d/e; bar[0]=NULL;
 660 gotoxy(0,3); printf("[%d%%]\n", 100*d/e);
 670 strcat(bar, "[");
 680 for(k=0;k<f;k++) strcat(bar, "*");
 690 for(k=0;k<22-f;k++) strcat(bar, "-");
 700 strcat(bar, "]");
 710 printf(bar);
 720 gotoxy(22,0); printf("%c", mark[d%4]);
 730 }
 740 }
 750 void title(){
 760 int key;
 770 clrscr();
 780 printf("\n\n     SILVER GLIDE");
 790 printf("\n\n     press 1 start");
 800 do{
 810 next++;
 820 key=getch();
 830 if(key =='1') break;
 840 }while(1);
 850 }
 860 void post(struct sprites *p,int a){
 870 int i;
 880 for(i=1;i<=31;i++){
 890 if(p[i].no == 0){
 900 p[i].no=1; p[i].x=144; p[i].y=a;
 910 break;
 920 }
 930 }
 940 }
 950 int game(struct sprites* p){
 960 int a,i,r,h,loop=1,s=0,t=20;
 970 float ex,ey;
 980 double px=40,py=28,w=60,g=0;
 990 unsigned char keybit=0;
1000 clrscr();
1010 printf("\nSILVER");
1020 printf("\n GLIDE");
1030 line(47, 0, 47, 47, 0, 0xffff, 0);
1040 p[0].no=2; p[0].x=px; p[0].y=py;
1050 for(i=1;i<=31;i++) p[i].no=0;
1060 while(loop==1){
1070 if(px < w) px+=1.0;
1080 if(w < 136) w+=0.075;
1090 outport(0x11, 0x20);
1100 keybit=inport(0x10);
1110 if((keybit & 0x08) != 0) g-=0.4;
1120 g+=0.14;
1130 py+=g;
1140 t--;
1150 if(t <= 0){
1160 t=28+get_random(36);
1170 r=get_random(100);
1180 if(r < 20){
1190 a=7+get_random(40);
1200 post(p, a);
1210 }else if(r < 60){
1220 a=7+get_random(32);
1230 post(p, a); a+=8; post(p, a);
1240 }else{
1250 a=7+get_random(26);
1260 post(p, a); a+=8;post(p,a); a+=8;post(p, a);
1270 }
1280 }
1290 h=0;
1300 for(i=1;i<=31;i++){
1310 if(p[i].no > 0){
1320 p[i].x-=1.0f; if(p[i].x <= 40) p[i].no=0;
1330 ex=p[i].x; ey=p[i].y;
1340 if(ex-6 < px && ex+6 > px && ey-6 < py && ey+6 > py) h=1;
1350 }
1360 }
1370 if(py < (-1) || py > 58) h=1;
1380 if(h != 0){
1390 p[0].no=3; loop=0;
1400 }
1410 p[0].x=px; p[0].y=py;
1420 if(s < 100000) s++;
1430 /*
1440 for(i=0;i<32;i++){
1450 mp[i*3+0]=p[i].no;
1460 mp[i*3+1]=p[i].x;
1470 mp[i*3+2]=p[i].y;
1480 }
1490 call(0x100, (void*)mp);
1500 */
1510 call(0x100, (void*)p);
1520 gotoxy(1, 4); printf("%05d", s);
1530 }
1540 wait_v(2000);
1550 return s;
1560 }
1570 int over(int s){
1580 int key;
1590 clrscr();
1600 printf("\n       Game Over");
1610 printf("\n         %05d",s);
1620 printf("\n\n      2:retry 3:end");
1630 do{
1640 key=getch();
1650 if(key =='2') return(0);
1660 if(key =='3') break;
1670 }while(1);
1680 clrscr();
1690 return(1);
1700 }
1710 static char *spr_data_internal[] = {
1720 "CD0A01CD4601CD1D01C9E521F2020100",
1730 "05160072230B78B1C21301E1C921F202",
1740 "01D8000916B07AD3403E00D3403E13D3",
1750 "403E60477E23D34110FA01400009147A",
1760 "FEB6C22601C9062022D302DD2AD302C5",
1770 "CD5802ED53CD02110400DD19CD5802ED",
1780 "53CF02110400DD19CD5802ED53D10211",
1790 "0400DD19ED5BCD027BFE00CA98013D5F",
1800 "1600CB23CB12CB23CB12CB23CB1221DA",
1810 "021922CD02CD9C01C110B4C92ACF0211",
1820 "080019CB7CC257021160FF19CB7CCA57",
1830 "0211A0001922D3022AD10211090019CB",
1840 "7CC2570211C0FF19CB7CCA5702113700",
1850 "1922D502CB2CCB1DCB2CCB1DCB2CCB1D",
1860 "7D2AD302FE00CAF0014711A0001910FD",
1870 "ED5BD5027BE607FE07C2130211F20219",
1880 "11A00019ED5BCD0206081A4EB1771323",
1890 "10F8C93C32D70211F20219FD2ACD020E",
1900 "00FD56001E00FD233AD70247CB22CB13",
1910 "10FA7A32D8027B32D9023AD802577EB2",
1920 "7711A000193AD902577EB2771161FF19",
1930 "0C79FE08C22102C9DD7E01E6F0FE20CA",
1940 "9102FE10CA7602DD7E020F0F0F0FE60F",
1950 "16005FC3BB02DD7E020F0F0F0FE60F4F",
1960 "878781874FDD7E02E60F8116005FC3BB",
1970 "02DD7E030F0F0F0FE60F6FDD7E02E60F",
1980 "4F878781878526006FDD7E020F0F0F0F",
1990 "E60F471164001910FD545DDD7E01E608",
2000 "FE00CACC027A2F577B2F5F13C9000000",
2010 "0000009700170004F00FFF81FFFF81FF",
2020 "81FF577E7C5C4C0C0C08539672595C8C",
2030 "2E090000000000000000000000000000",
2040 NULL
2050 };
2060 char** get_spr_data(){ return spr_data_internal; }
