
 /* マシン語展開開始アドレス */
#define ADDR_START 0x100

struct sprites{
 float no;
 float x;
 float y;
};
/* 疑似乱数を生成するための変数（シード値） */
unsigned long next = 1;

/* プロトタイプ宣言 */
void load();
void title();
int game(struct sprites*);
int over(int);
char** get_spr_data();

/* --- ＭＡＩＮ --- */
main(){
 struct sprites p[32];
 int s=0;

 load();
 title();
 do{
  s=game(p); 
  if(over(s) != 0) break;
 }while(1);
}

/* 待機用関数 (BASICのWAIT相当) */
void wait_v(t){
 int i=0;
 for(i=0;i<t;i++);
}
/* 乱数の生成数 */
int my_rand(void) {
    /* 線形合同法 */
    next = next*1103515245+12345;
    return (unsigned int)(next/65536)%32768;
}
/* 0 ～ max-1 の範囲で乱数取得 */
int get_random(int max) {
    if (max <= 0) return 0;
    return my_rand() % max;
}
/* 16進数文字を数値に変換 */
unsigned char hex2val(char c){
 if (c >='0' && c <='9') return c-'0';
 if (c >='A' && c <='F') return c-'A'+10;
 if (c >='a' && c <='f') return c-'a'+10;
 return 0;
}
/* 2文字の16進数文字列を1バイトに変換 */
unsigned char h2b(const char *s) {
 return (hex2val(s[0]) << 4) | hex2val(s[1]);
}
/* --- 初期ロード --- */
void load(){
 unsigned int ptr=ADDR_START;
 int i,j,k,key,c=0x100,d=0,e=33,f=0;
 char bar[24],mark[4]={'/','-','|','.'};
 char** spr_data = get_spr_data();

 clrscr();
 printf("\n    1:Load  2:Skip\n\n Initial load required");
 do{
  next++;
  key=getch();
  if(key =='1') break;
  if(key =='2') return;
 }while(1);

 clrscr();
 printf("WRITING MACHINE CODE..\n");
 printf("WAIT ABOUT 10 SECONDS\n");
 printf("KEEP POWER ON!\n");
 
 for(i=0;spr_data[i] != NULL;i++){
  for (j=0; spr_data[i][j] != '\0';j+=2){
   poke(ptr++, h2b(&spr_data[i][j]));
  }
  d++; f=22*d/e; bar[0]=NULL;
  gotoxy(0,3); printf("[%d%%]\n", 100*d/e);
  strcat(bar, "[");
  for(k=0;k<f;k++) strcat(bar, "*");
  for(k=0;k<22-f;k++) strcat(bar, "-");
  strcat(bar, "]");
  printf(bar);
  gotoxy(22,0); printf("%c", mark[d%4]);
 }
}

/* --- タイトル画面 --- */
void title(){
 int key;
 clrscr();
 printf("\n\n     SILVER GLIDE");
 printf("\n\n     press 1 start");
 do{
  next++;
  key=getch();
  if(key =='1') break;
 }while(1);
}

/* --- 柱１マス生成 --- */
void post(struct sprites *p,int a){
 int i;
 for(i=1;i<=31;i++){
  if(p[i].no == 0){
   p[i].no=1; p[i].x=144; p[i].y=a;
   break;
  }
 }
}
/* --- ゲーム本体 --- */
int game(struct sprites* p){
 int a,i,r,h,loop=1,s=0,t=20;
 float ex,ey;
 double px=40,py=28,w=60,g=0;
 unsigned char keybit=0;
 //short mp[32*3];
 
 clrscr();
 printf("\nSILVER");
 printf("\n GLIDE");
 line(47, 0, 47, 47, 0, 0xffff, 0);

 p[0].no=2; p[0].x=px; p[0].y=py;
 for(i=1;i<=31;i++) p[i].no=0;

 /* --- ゲームメインループ --- */
 while(loop==1){
  /* --- 自機挙動 --- */
  if(px < w) px+=1.0;
  if(w < 136) w+=0.075;

  outport(0x11, 0x20);
  keybit=inport(0x10);  // 「１」キー入力検知
  if((keybit & 0x08) != 0) g-=0.4;
  g+=0.14;
  py+=g;

  /* --- ランダムで柱出現（位置、サイズもランダム）--- */
  t--;
  if(t <= 0){
   t=28+get_random(36);
   r=get_random(100);
   if(r < 20){
    a=7+get_random(40);
    post(p, a);
   }else if(r < 60){
    a=7+get_random(32);
    post(p, a); a+=8; post(p, a);
   }else{
    a=7+get_random(26);
    post(p, a); a+=8;post(p,a); a+=8;post(p, a);
   }
  }
  /* --- 存在する柱の移動。自機との当り判定 --- */
  h=0;
  for(i=1;i<=31;i++){
   if(p[i].no > 0){
    p[i].x-=1.0f; if(p[i].x <= 40) p[i].no=0;
    ex=p[i].x; ey=p[i].y;
    if(ex-6 < px && ex+6 > px && ey-6 < py && ey+6 > py) h=1;
   }
  }
  /* --- 自機上下画面外判定 --- */
  if(py < (-1) || py > 58) h=1;
  if(h != 0){
   p[0].no=3; loop=0; /* 死亡判定 */
  }

  p[0].x=px; p[0].y=py;
  if(s < 100000) s++;
  /* VRAM式スプライト描画 */
  /*
  for(i=0;i<32;i++){
   mp[i*3+0]=p[i].no;
   mp[i*3+1]=p[i].x;
   mp[i*3+2]=p[i].y;
  }
  call(0x100, (void*)mp);
  */
  call(0x100, (void*)p);
  /* 点数表示 */
  gotoxy(1, 4); printf("%05d", s);
 }

 wait_v(2000);
 return s;
}

/* --- GAME OVER画面 --- */
int over(int s){
 int key;
 clrscr();
 printf("\n       Game Over");
 printf("\n         %05d",s);
 printf("\n\n      2:retry 3:end");
 do{
  key=getch();
  if(key =='2') return(0);
  if(key =='3') break;
 }while(1);

 clrscr();
 return(1);
}

/* --- スプライトシステム機械語 --- */
static char *spr_data_internal[] = {
    "CD0A01CD4601CD1D01C9E521F2020100",
    "05160072230B78B1C21301E1C921F202",
    "01D8000916B07AD3403E00D3403E13D3",
    "403E60477E23D34110FA01400009147A",
    "FEB6C22601C9062022D302DD2AD302C5",
    "CD5802ED53CD02110400DD19CD5802ED",
    "53CF02110400DD19CD5802ED53D10211",
    "0400DD19ED5BCD027BFE00CA98013D5F",
    "1600CB23CB12CB23CB12CB23CB1221DA",
    "021922CD02CD9C01C110B4C92ACF0211",
    "080019CB7CC257021160FF19CB7CCA57",
    "0211A0001922D3022AD10211090019CB",
    "7CC2570211C0FF19CB7CCA5702113700",
    "1922D502CB2CCB1DCB2CCB1DCB2CCB1D",
    "7D2AD302FE00CAF0014711A0001910FD",
    "ED5BD5027BE607FE07C2130211F20219",
    "11A00019ED5BCD0206081A4EB1771323",
    "10F8C93C32D70211F20219FD2ACD020E",
    "00FD56001E00FD233AD70247CB22CB13",
    "10FA7A32D8027B32D9023AD802577EB2",
    "7711A000193AD902577EB2771161FF19",
    "0C79FE08C22102C9DD7E01E6F0FE20CA",
    "9102FE10CA7602DD7E020F0F0F0FE60F",
    "16005FC3BB02DD7E020F0F0F0FE60F4F",
    "878781874FDD7E02E60F8116005FC3BB",
    "02DD7E030F0F0F0FE60F6FDD7E02E60F",
    "4F878781878526006FDD7E020F0F0F0F",
    "E60F471164001910FD545DDD7E01E608",
    "FE00CACC027A2F577B2F5F13C9000000",
    "0000009700170004F00FFF81FFFF81FF",
    "81FF577E7C5C4C0C0C08539672595C8C",
    "2E090000000000000000000000000000",
    NULL
};
char** get_spr_data(){ return spr_data_internal; }
