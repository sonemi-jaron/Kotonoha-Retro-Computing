10 main() {
20     int x, y, s, o, h, t;
30     int keybit;
40     clrscr();
50     printf("SILVER GLIDE 1\n");
60     printf("PRESS 1 KEY TO START\n");
70     while(1) {
80         /* スタート待ちもI/Oポートで「1」キー入力を監視 */
90         outport(0x11, 0x20);
100        keybit = inport(0x10);
110        if((keybit & 0x08) != 0) break; /* 「1」キー検知で開始 */
115    }
120    clrscr();
125 RESTART:
130    x = 20; y = 24; s = 0; o = 143; h = 16; t = 0;
140    gotoxy(0, 0); printf("SCORE:%d", s);
150    while(1) {
160        /* 【対策】自機消去：全ドットが0のデータを同じ座標にgprintして消す */
170        gcursor(x, y);
180        gprint("0000000000000000");
190        
200        /* 【対策】I/Oポートによるリアルタイム入力処理 */
210        outport(0x11, 0x20);   /* 「1」キーがある行を選択 */
220        keybit = inport(0x10);  /* キーの状態を読み込み */
230        
240        if((keybit & 0x08) != 0) {
250            y = y - 4; /* 「1」キーが押されていたら上昇 */
260        } else {
270            y = y + 2; /* 離していたら自由落下 */
280        }
290        if(y < 8) y = 8;
300        if(y > 47) break; /* GAME OVERへ */
310        
320        /* 自機描画 (ドット絵) */
330        gcursor(x, y);
340        gprint("4040F0F0E0E08000");
350        
360        /* 柱の制御と描画 */
370        line(o, 8, o, 47, 1, 0xFFFF, 0); /* 前回の柱全体を消去 */
380        o = o - 4;
390        if(o <= 0) {
400            o = 143; 
410            h = 16; 
420            t = 0;  
430            s = s + 1;
440            /* 【対策】文字ズレ防止のため、描画前に必ずgotoxyで座標をリセット */
450            gotoxy(0, 0); printf("SCORE:%d", s);
460        }
470        
480        /* 柱の描画 */
490        if(t == 0) {
500            line(o, 8, o, 8+h, 0, 0xFFFF, 0);
510        } else {
520            line(o, 47-(h-4), o, 47, 0, 0xFFFF, 0);
530        }
540        
550        /* 当たり判定 */
560        if(o >= x && o <= x+7) {
570            if(t == 0 && y < 8+h) break;
580            if(t == 1 && y > 47-(h-4)-7) break;
590        }
600    }
610    clrscr();
620    printf("GAME OVER\n");
630    printf("SCORE:%d\n", s);
640    printf("PRESS 1 TO RESTART\n");
650    while(1) { 
660        /* ゲームオーバー画面からの復帰もI/Oポートで監視 */
670        outport(0x11, 0x20);
680        keybit = inport(0x10);
690        if((keybit & 0x08) != 0) break;
700    }
710    goto RESTART;
720 }