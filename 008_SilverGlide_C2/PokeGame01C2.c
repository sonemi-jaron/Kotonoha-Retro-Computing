
 
#define ADDR_START 0x100 /* マシン語展開開始アドレス */
#define OBJ_NUM 32
#define TYPE_NONE 0
#define TYPE_PLAYER 1
#define TYPE_POST 2
#define TYPE_SHOT 3
#define TYPE_BULT 4
#define TYPE_ENM1 5
#define TYPE_ENM2 6
#define TYPE_ENM3 7
#define TYPE_ENM4 8

/* 疑似乱数を生成するための変数（シード値） */
unsigned long next = 1;

/* プロトタイプ宣言 */
void load();
void title();
void game();
int over();
char** get_spr_data();

/* グローバル変数 */
float spr[OBJ_NUM][3]; /* no,x,y */
unsigned char type[OBJ_NUM];
unsigned char step[OBJ_NUM];
unsigned char flag[OBJ_NUM];
float delX[OBJ_NUM], delY[OBJ_NUM];
double px,py,pa;
float sx,sy,dx,dy;
int score;

/* --- ＭＡＩＮ --- */
main(){
 load();
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
 int i,j,k,key,d=0,e=35,f=0;
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
 int j;
 for(j=1;j<OBJ_NUM;j++){
  if(type[j] == TYPE_POST || (type[j] >= TYPE_ENM1 && step[j] < 90)){
   dx=spr[j][1]; dy=spr[j][2];
   if(dx-6 < sx && dx+6 > sx && dy-8 < sy && dy+8 > sy){
    if(type[j] >= TYPE_ENM1){
     score+=100;
     step[j]=90;
    }
    spr[i][1]=px+4; spr[i][2]=py; /* 再発生 */
   }
  }
 }
}
/* --- 柱１マス生成 --- */
void post(int y){
 int i=getObjNo();
 if(i > 0){
  type[i]=TYPE_POST;
  spr[i][0]=1; spr[i][1]=144; spr[i][2]=y;
 }
}
/* --- 敵弾生成 --- */
void bullet(int way){
 int i=getObjNo();
 if(i > 0){
  type[i]=TYPE_BULT;
  spr[i][0]=5; spr[i][1]=sx; spr[i][2]=sy;
  switch(way){
   case 0:
    delX[i]=-2.5; delY[i]=(py-sy)/60.0;
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
 int i,j,r,y,loop=1,enmInt=20,plyHit=0;
 float enmRnd=28;
 unsigned char keybit=0;
 score=0;
 
 clrscr();
 printf("\nSILVER");
 printf("\n GLIDE");
 line(47, 0, 47, 47, 0, 0xffff, 0);

 /* 初期化 */
 px=40.0; py=28.0; pa=0.0;
 for(i=0;i<OBJ_NUM;i++){
  type[i]=TYPE_NONE; step[i]=0;
 }
 type[0]=TYPE_PLAYER; spr[0][0]=2; spr[0][1]=px; spr[0][2]=py;

 /* --- ゲームメインループ --- */
 while(loop==1){
  plyHit=0;
  for(i=0;i<OBJ_NUM;i++){
   if(type[i] > TYPE_NONE){
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
         spr[j][0]=4; spr[j][1]=px+4; spr[j][2]=py;
         step[i]++;
        }
        break;
       case 1:
        outport(0x11, 0x40);
        keybit=inport(0x10);
        if((keybit & 0x38) != 0) pa-=0.4; /*「４,５,６」キー入力検知 上へ加速*/
        outport(0x11, 0x20);
        keybit=inport(0x10);
        if((keybit & 0x38) != 0) pa+=0.4; /*「１,２,３」キー入力検知 下へ加速*/
        py+=pa;
        if(py < (-1) || py > 58) plyHit=1; /* 画面外判定 */
        break;
      }
      spr[i][0]=2; spr[i][1]=px; spr[i][2]=py;
      break;
     /* --- 柱 --- */
     case TYPE_POST:
      spr[i][0]=1; spr[i][1]-=1.0f; if(spr[i][1] <= 40) type[i]=TYPE_NONE;
      dx=spr[i][1]; dy=spr[i][2];
      if(dx-6 < px && dx+6 > px && dy-6 < py && dy+6 > py) plyHit=1;
      break;
     /* --- ショット --- */
     case TYPE_SHOT:
      spr[i][1]+=delX[i];
      sx=spr[i][1]; sy=spr[i][2];
      if(sx < 144){
       shotHit(i);
      }else{
       spr[i][1]=px+4; spr[i][2]=py; /* 再発生 */
      }
      break;
     /* --- 敵弾 --- */
     case TYPE_BULT:
      spr[i][1]+=delX[i]; spr[i][2]+=delY[i];
      dx=spr[i][1]; dy=spr[i][2];
      if(dx >= 40 && dy >= 0 && dy < 56){
       if(dx-2 < px && dx+2 > px && dy-2 < py && dy+2 > py){
        plyHit=1;
        type[i]=TYPE_NONE;
       }
      }else{
       type[i]=TYPE_NONE;
      }
      break;
     /* --- 敵１ --- */
     case TYPE_ENM1:
      spr[i][1]+=delX[i]; spr[i][2]+=delY[i];
      sx=spr[i][1]; sy=spr[i][2];
      if(step[i] < 90){
         if(sx-4 < px && sx+4 > px && sy-4 < py && sy+4 > py){
          spr[i][0]=3; plyHit=1;
         }
      }
      switch(step[i]){
       case 0:
        if(sx < 72){
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
        if(sx >= 144) type[i]=TYPE_NONE;
        break;
       case 90:
        spr[i][0]=3; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
     /* --- 敵２ --- */
     case TYPE_ENM2:
      spr[i][1]+=delX[i];
      sx=spr[i][1]; sy=spr[i][2];
      switch(step[i]){
       case 0:
        if(sx >= 40){
         dx=spr[i][1]; dy=spr[i][2];
         if(dx-4 < px && dx+4 > px && dy-4 < py && dy+4 > py){
          spr[i][0]=3; plyHit=1;
         }
        }else{
         type[i]=TYPE_NONE;
        }
        break;
       case 90:
        spr[i][0]=3; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
     /* --- 敵３ --- */
     case TYPE_ENM3:
      spr[i][1]+=delX[i];
      sx=spr[i][1]; sy=spr[i][2];
      switch(step[i]){
       case 0:
        if(sx >= 40){
         dx=spr[i][1]; dy=spr[i][2];
         if(dx-4 < px && dx+4 > px && dy-4 < py && dy+4 > py){
          spr[i][0]=3; plyHit=1;
         }
         flag[i]++;
         if(flag[i] > 20){
          flag[i]=0;
          bullet(1); bullet(2); bullet(3);
         }
        }else{
         type[i]=TYPE_NONE;
        }
        break;
       case 90:
        spr[i][0]=3; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
     /* --- 敵４（敵２弾撃ち版） --- */
     case TYPE_ENM4:
      spr[i][1]+=delX[i];
      sx=spr[i][1]; sy=spr[i][2];
      switch(step[i]){
       case 0:
        if(sx >= 40){
         dx=spr[i][1]; dy=spr[i][2];
         if(dx-4 < px && dx+4 > px && dy-4 < py && dy+4 > py){
          spr[i][0]=3; plyHit=1;
         }
         if(flag[i] == 0 && sx < 120){
          flag[i]=1;
          bullet(0);
         }
        }else{
         type[i]=TYPE_NONE;
        }
        break;
       case 90:
        spr[i][0]=3; step[i]++;
        break;
       case 91:
        type[i]=TYPE_NONE;
        break;
      }
      break;
    }
   }else{
    spr[i][0]=0;
   }
  }
  /* --- ランダムで敵/柱出現 --- */
  enmInt--;
  if(enmInt <= 0){
   enmInt=enmRnd+get_random(enmRnd);
   if(enmRnd > 11) enmRnd-=0.5;
   r=get_random(120);
   if(r < 10){ /* 柱１個 */
    y=7+get_random(40);
    post(y);
   }else if(r < 20){ /* 柱２個 */
    y=7+get_random(32);
    post(y); y+=8; post(y);
   }else if(r < 50){ /* 敵１ */
    y=10+get_random(38);
    i=getObjNo();
    if(i > 0){
     type[i]=TYPE_ENM1; step[i]=0; delX[i]=-2.8; delY[i]=-0.5;
     spr[i][0]=6; spr[i][1]=144; spr[i][2]=y;
    }
   }else if(r < 80){ /* 敵２ */
    y=7+get_random(40);
    i=getObjNo();
    if(i > 0){
     type[i]=TYPE_ENM2; step[i]=0; delX[i]=-1.5; delY[i]=0;
     spr[i][0]=7; spr[i][1]=144; spr[i][2]=y;
    }
   }else if(r < 90){ /* 敵３ */
    y=7+get_random(40);
    i=getObjNo();
    if(i > 0){
     type[i]=TYPE_ENM3; step[i]=0; delX[i]=-1.0; delY[i]=0;
     spr[i][0]=8; spr[i][1]=144; spr[i][2]=y;
    }

   }else{            /* 敵４ */
    y=7+get_random(40);
    i=getObjNo();
    if(i > 0){
     type[i]=TYPE_ENM4; step[i]=0; delX[i]=-1.5; delY[i]=0;
     spr[i][0]=7; spr[i][1]=144; spr[i][2]=y;
    }
   }
  }
  /* 死亡判定 */
  if(plyHit != 0){
   spr[0][0]=3; loop=0;
  }
 
  /* VRAM式スプライト描画 */
  call(ADDR_START, (void*)spr);
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
    "CD0A01CD4601CD1D01C9E5211A030100",
    "05160072230B78B1C21301E1C9211A03",
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
    "ED5BD5027BE607FE07C21302111A0319",
    "11A00019ED5BCD0206081A4EB1771323",
    "10F8C93C32D702111A0319FD2ACD020E",
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
    "00000000000000000000FF81FFFF81FF",
    "81FF577E7C5C4C0C0C08539672595C8C",
    "2E09000081C37E3C00000018245A5A24",
    "18008C52767E3C6E614060D2D3F3F37F",
    "7E3840C8ECF6F7FFFE78000000000000",
    NULL
};
char** get_spr_data(){ return spr_data_internal; }
