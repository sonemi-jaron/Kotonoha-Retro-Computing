
 
#define ADDR_START 0x100 /* マシン語展開開始アドレス */
#define ADDR_SPR_PRM 0x104
#define ADDR_SPR_POS 0x106

#define OBJ_NUM 32

#define TYPE_NONE 0
#define TYPE_PLAYER 1
#define TYPE_SHOT 2
#define TYPE_BULT 3
#define TYPE_ENM1 4
#define TYPE_ENM2 5
#define TYPE_ENM3 6
#define TYPE_ENM4 7
#define TYPE_POST 8
#define TYPE_POST2 9
#define TYPE_BOSS 10

/* 疑似乱数を生成するための変数（シード値） */
unsigned long next = 1;

/* プロトタイプ宣言 */
void load();
void title();
void game();
int over();
char** get_spr_data();

/* グローバル変数 */
//float spr[OBJ_NUM][3]; /* no,x,y */
unsigned char sprPrm[OBJ_NUM][3]; /* no,w,h */
float sprPos[OBJ_NUM][2]; /* x,y */
unsigned char type[OBJ_NUM];
unsigned char step[OBJ_NUM];
unsigned char flag[OBJ_NUM];
float delX[OBJ_NUM], delY[OBJ_NUM];
double px,py,pa;
float sx,sy,ex,ey;
int score;
int plyHit;
int bossHP;
short hl=0;

/* 指定メモリにアドレスをPOKE */
void pokeAdr(unsigned int mem, void* adr){
 unsigned short a=(unsigned short)adr;
 poke(mem+0, a&0xff);
 poke(mem+1, (a>>8)&0xff);
}

/* --- ＭＡＩＮ --- */
main(){
 load();
 pokeAdr(ADDR_SPR_PRM, sprPrm);
 pokeAdr(ADDR_SPR_POS, sprPos);
 title();
 do{
  game(); 
  if(over() != 0) break;
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
 int i,j,k,key,d=0,e=43,f=0;
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

/* --- 空オブジェ検索 --- */
int getObjNo(){
 int i;
 for(i=1;i<32;i++){
  if(type[i] == TYPE_NONE){
   step[i]=0; flag[i]=0;
   return(i);
  }
 }
 return(-1);
}
/* --- ショットHIT処理 --- */
void shotHit(int i){
 int j,w,h;
 for(j=1;j<OBJ_NUM;j++){
  if(type[j] == TYPE_POST || (type[j] >= TYPE_ENM1 && step[j] < 90)){
   ex=sprPos[j][0]; ey=sprPos[j][1];
   w=6+(sprPrm[j][1]-1)*8;
   h=sprPrm[j][2]*8;
   if(ex-6 < sx && ex+w > sx && ey-8 < sy && ey+h > sy){
    if(type[j] < TYPE_POST){
     score+=100;
     step[j]=90;
    }else if(type[j]==TYPE_BOSS){
     bossHP--;
     if(bossHP <= 0){
      score+=3000;
      step[j]=90;
     }
    }
    sprPos[i][0]=px+4; sprPos[i][1]=py; /* 再発生 */
   }
  }
 }
}
/* --- ザコ敵HIT処理 --- */
int enmHit(int i){
 ex=sprPos[i][0]; ey=sprPos[i][1];
 if(ex-4 < px && ex+4 > px && ey-4 < py && ey+4 > py){
  sprPrm[i][0]=2; step[i]=90; plyHit=1;
 }
}
/* --- ザコ敵/柱生成 --- */
int makeEnm(int t,int n, float y, float dx, float dy){
 int i=getObjNo();
 if(i > 0){
  type[i]=t; step[i]=0;
  sprPrm[i][0]=n; sprPrm[i][1]=1; sprPrm[i][2]=1;
  sprPos[i][0]=144; sprPos[i][1]=y;
  delX[i]=dx; delY[i]=dy;
 }
 return i;
}
/* --- 敵弾生成 --- */
void bullet(int way){
 int i=getObjNo();
 if(i > 0){
  type[i]=TYPE_BULT;
  sprPrm[i][0]=4; sprPrm[i][1]=1; sprPrm[i][2]=1;
  sprPos[i][0]=ex; sprPos[i][1]=ey;
  switch(way){
   case 0:
    delX[i]=-2.5; delY[i]=(py-ey)/60.0;
    break;
   case 1:
    delX[i]=-2.5; delY[i]=0;
    break;
   case 2:
    delX[i]=-2.5; delY[i]=1.25;
    break;
   case 3:
    delX[i]=-2.5; delY[i]=-1.25;
    break;
  }
 }
}

/* --- ゲーム本体 --- */
void game(){
 int i,j,r,y,loop=1,enmInt=20,enmCnt=20,bossF=0;
 float enmRnd=28;
 unsigned char keybit=0;
 score=0;
 plyHit=0;
 
 clrscr();
 printf("\nSILVER");
 printf("\n GLIDE");
 line(47, 0, 47, 47, 0, 0xffff, 0);

 /* 初期化 */
 px=40.0; py=28.0; pa=0.0;
 for(i=0;i<OBJ_NUM;i++){
  type[i]=TYPE_NONE; step[i]=0;
  sprPrm[i][0]=0; sprPrm[i][1]=1; sprPrm[i][2]=1;
 }
 type[0]=TYPE_PLAYER; sprPrm[0][0]=1; sprPos[0][0]=px; sprPos[0][1]=py;

 /* --- ゲームメインループ --- */
 while(loop==1){
  plyHit=0;
  for(i=0;i<OBJ_NUM;i++){
   if(type[i] > TYPE_NONE){
    if(type[i] != TYPE_PLAYER){
     sprPos[i][0]+=delX[i]; sprPos[i][1]+=delY[i];
    }
    switch(type[i]){
     /* --- 自機 --- */
     case TYPE_PLAYER:
      switch(step[i]){
       case 0:
        px+=1.5;
        if(px >= 60){
         /* ショット生成 */
         j=getObjNo();
         type[j]=TYPE_SHOT; delX[j]=5;
         sprPrm[j][0]=3; sprPos[j][0]=px+4; sprPos[j][1]=py;
         step[i]++;
        }
        break;
       case 1:
        outport(0x11, 0x40);
        keybit=inport(0x10);
        if((keybit & 0x38) != 0){ /*「４,５,６」キー入力検知 上へ加速*/
         pa-=0.4; get_random(100); /*（ゲームのランダム性を上げる為、無意味にランダム値生成）*/
        }
        outport(0x11, 0x20);
        keybit=inport(0x10);
        if((keybit & 0x38) != 0){ /*「１,２,３」キー入力検知 下へ加速*/
         pa+=0.4; get_random(100); /*（ゲームのランダム性を上げる為、無意味にランダム値生成）*/
        }
        py+=pa;
        if(py < (-1) || py > 58) plyHit=1; /* 画面外判定 */
        break;
      }
      sprPos[i][0]=px; sprPos[i][1]=py;
      break;
     /* --- 柱 --- */
     case TYPE_POST:
      if(sprPos[i][0] <= 40) type[i]=TYPE_NONE;
      ex=sprPos[i][0]; ey=sprPos[i][1];
      if(ex-6 < px && ex+6 > px && ey-6 < py && ey+6 > py) plyHit=1;
      break;
     /* --- 柱２（縦長） --- */
     case TYPE_POST2:
      if(sprPos[i][0] <= 40) type[i]=TYPE_NONE;
      ex=sprPos[i][0]; ey=sprPos[i][1];
      if(ex-6 < px && ex+6 > px && ey-6 < py && ey+14 > py) plyHit=1;
      break;
     /* --- ショット --- */
     case TYPE_SHOT:
      sx=sprPos[i][0]; sy=sprPos[i][1];
      if(sx < 144){
       shotHit(i);
      }else{
       sprPos[i][0]=px+4; sprPos[i][1]=py; /* 再発生 */
      }
      break;
     /* --- 敵弾 --- */
     case TYPE_BULT:
      ex=sprPos[i][0]; ey=sprPos[i][1];
      if(ex >= 40 && ey >= 0 && ey < 56){
       if(ex-2 < px && ex+2 > px && ey-2 < py && ey+2 > py){
        plyHit=1;
        type[i]=TYPE_NONE;
       }
      }else{
       type[i]=TYPE_NONE;
      }
      break;
     /* --- 敵１ --- */
     case TYPE_ENM1:
      if(step[i] < 90){
       enmHit(i);
      }
      switch(step[i]){
       case 0:
        if(sprPos[i][0] < 72){
         delY[i]= -delY[i];
         step[i]++;
        }
        break;
       case 1:
        delX[i]+=0.2;
        if(delX[i] >= 2.0){
         delY[i]= -delY[i];
         step[i]++;
        }
        break;
       case 2:
        if(sprPos[i][0] >= 144) type[i]=TYPE_NONE;
        break;
       case 90:
        sprPrm[i][0]=2; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
     /* --- 敵２ --- */
     case TYPE_ENM2:
      switch(step[i]){
       case 0:
        if(sprPos[i][0] >= 40){
         enmHit(i);
        }else{
         type[i]=TYPE_NONE;
        }
        break;
       case 90:
        sprPrm[i][0]=2; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
     /* --- 敵３ --- */
     case TYPE_ENM3:
      switch(step[i]){
       case 0:
        if(sprPos[i][0] >= 40){
         enmHit(i);
         flag[i]++;
         if(flag[i] > 30){
          flag[i]=0;
          bullet(1); bullet(2); bullet(3);
         }
        }else{
         type[i]=TYPE_NONE;
        }
        break;
       case 90:
        sprPrm[i][0]=2; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
     /* --- 敵４（敵２弾撃ち版） --- */
     case TYPE_ENM4:
      switch(step[i]){
       case 0:
        if(sprPos[i][0] >= 40){
         enmHit(i);
         if(flag[i] == 0 && sprPos[i][0] < 120){
          flag[i]=1;
          bullet(0);
         }
        }else{
         type[i]=TYPE_NONE;
        }
        break;
       case 90:
        sprPrm[i][0]=2; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
     /* --- ボス --- */
     case TYPE_BOSS:
      if(step[i] < 90){
       enmHit(i);
      }
      switch(step[i]){
       case 0:
        if(sprPos[i][0] < 112){
         delX[i]=0; delY[i]=-1.2;
         step[i]++;
        }
        break;
       case 1:
        delY[i]+=0.1;
        if(delY[i] >= 1.2){
         bullet(0); bullet(2); bullet(3);
         step[i]++;
        }
        break;
       case 2:
        delY[i]-=0.1;
        if(delY[i] <=-1.2){
         bullet(0); bullet(2); bullet(3);
         step[i]--;
        }
        break;
       case 90:
        delY[i]=1.0; step[i]++;
       case 91:
        if(sprPos[i][1] <= 64){
         j=(int)sprPos[i][1];
         sprPrm[i][0]=10;
         if(j%2 == 0) sprPrm[i][0]=0;
        }else{
         bossF=0; type[i]=TYPE_NONE;
        }
        break;
      }
      break;
    }
   }else{
    sprPrm[i][0]=0;
   }
  }
  /* --- ランダムで敵/柱出現 --- */
  enmInt--;
  if(enmInt <= 0){
   if(bossF == 0){
    enmCnt--;
    if(enmCnt <= 0){
     i=makeEnm(TYPE_BOSS, 10, 24,-1.0, 0);
     if(i > 0) {
      sprPrm[i][1]=3;
      sprPrm[i][2]=2;
      enmCnt=20;
      bossHP=20;
      bossF=1;
     }
    }
   }
   enmInt=enmRnd+get_random(enmRnd);
   if(enmRnd > 11) enmRnd-=0.5;
   r=get_random(120);
   if(r < 10){ /* 柱１個 */
    y=7+get_random(40);
    makeEnm(TYPE_POST, 8, y,-1.25, 0);
   }else if(r < 20){ /* 柱２個 */
    y=7+get_random(32);
    i=makeEnm(TYPE_POST, 8, y,-1.0, 0);
    if(i > 0) sprPrm[i][2]=2;
   }else if(r < 50){ /* 敵１ */
    y=10+get_random(38);
    makeEnm(TYPE_ENM1, 5, y,-2.8,-0.5);
   }else if(r < 80){ /* 敵２ */
    y=7+get_random(40);
    makeEnm(TYPE_ENM2, 6, y,-1.5, 0);
   }else if(r < 90){ /* 敵３ */
    y=7+get_random(40);
    makeEnm(TYPE_ENM3, 7, y,-1.0, 0);
   }else{            /* 敵４ */
    y=7+get_random(40);
    makeEnm(TYPE_ENM4, 6, y,-1.5, 0);
    i=getObjNo();
   }
  }
  /* 死亡判定 */
  if(plyHit != 0){
   sprPrm[0][0]=2; loop=0;
  }
 
  /* VRAM式スプライト描画 */
  call(ADDR_START, &hl);
  
  /* 点数表示 */
  if(score < 1000000) score++;
  gotoxy(0, 4); printf("%06d", score);
 }

 wait_v(2000);
 return;
}

/* --- GAME OVER画面 --- */
int over(){
 int key;
 clrscr();
 printf("\n       Game Over");
 printf("\n        %06d",score);
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
const char *spr_data_internal[] = {
    "C30801CD00000000CD1201CD4C01CD23",
    "01C921A703010005160072230B78B1C2",
    "1A01C921A70301D8000916B07AD3403E",
    "00D3403E13D3403E60477E23D34110FA",
    "01400009147AFEB6C22C01C90620FD2A",
    "0401DD2A0601C5FD7E00321D03FD7E01",
    "321E03FD7E02321F03110300FD19CDA8",
    "02ED532503110400DD19CDA802ED5327",
    "03110400DD193A1D03FE00CADC013D5F",
    "1600CB23CB12CB23CB12CB23CB12212F",
    "03192223032A25032229033A1F0347C5",
    "3A1E0347C5CDE2011108002A23031922",
    "23032A250319222503C110E82A290322",
    "25032A270319222703C110D3C105C256",
    "01C9DDE5FDE52A250311080019CB7CC2",
    "A3021160FF19CB7CCAA30211A0001922",
    "2B032A270311090019CB7CC2A30211C0",
    "FF19CB7CCAA30211370019222D03CB2C",
    "CB1DCB2CCB1DCB2CCB1D7D2A2B03FE00",
    "CA3A024711A0001910FDED5B2D037BE6",
    "07FE07C25F0211A7031911A00019ED5B",
    "230306081A4EB177132310F8C3A3023C",
    "32200311A70319FD2A23030E00FD5600",
    "1E00FD233A200347CB22CB1310FA7A32",
    "21037B3222033A2103577EB27711A000",
    "193A2203577EB2771161FF190C79FE08",
    "C26D02FDE1DDE1C9DD7E01E6F0FE20CA",
    "E102FE10CAC602DD7E020F0F0F0FE60F",
    "16005FC30B03DD7E020F0F0F0FE60F4F",
    "878781874FDD7E02E60F8116005FC30B",
    "03DD7E030F0F0F0FE60F6FDD7E02E60F",
    "4F878781878526006FDD7E020F0F0F0F",
    "E60F471164001910FD545DDD7E01E608",
    "FE00CA1C037A2F577B2F5F13C9000000",
    "00000000000000000000000000000057",
    "7E7C5C4C0C0C08539672595C8C2E0900",
    "0081C37E3C00000018245A5A2418008C",
    "52767E3C6E614060D2D3F3F37F7E3840",
    "C8ECF6F7FFFE78FF81FFFF81FF81FFFF",
    "81FFFF81FF81FF80C040C0C0C0C0C0C0",
    "E0F0F8FCFCCCCCC8E0F0301080C0C003",
    "070F0A0F0F0F1F1F3F0F0F0F0F0F0B1D",
    "3F3F2F272D0F0A000000000000000000",
    NULL
};
char** get_spr_data(){ return spr_data_internal; }
