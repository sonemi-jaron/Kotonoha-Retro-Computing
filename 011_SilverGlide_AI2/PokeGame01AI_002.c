10 main() {
20     int x, y, s, o, h, t;
30     char k;
40     clscr();
50     printf("SILVER GLIDE 1\n");
60     printf("PRESS 1 KEY TO START\n");
70     while(1) {
80         if(kbhit()) {
90             if(getch() == '1') break;
100        }
110    }
120    clscr();
130    x = 20; y = 24; s = 0; o = 143; h = 16; t = 0;
140    gcursor(0, 0); printf("SCORE:%d", s);
150    while(1) {
160        /* 自機消去 (4隅のドットを消去) */
170        /* 引数：int preset(int x, int y); */
180        preset(x, y); preset(x+7, y); preset(x, y+7); preset(x+7, y+7);
190        
200        /* 入力処理 */
210        if(kbhit()) {
220            k = getch();
230            if(k == '1') y = y - 4;
240            else y = y + 2;
250        } else {
260            y = y + 2;
270        }
280        if(y < 8) y = 8;
290        if(y > 47) break; /* GAME OVERへ */
300        
310        /* 自機描画 (ドット絵) */
320        gcursor(x, y);
330        gprint("4040F0F0E0E08000");
340        
350        /* 柱の制御と描画 */
360        /* 引数：(x1, y1, x2, y2, reverse, mask, rectangle) */
370        /* 390行目修正：前回の柱を消去 (reverse=1:リセット, mask=0xFFFF, rectangle=0:直線) */
380        line(o, 8, o, 47, 1, 0xFFFF, 0); 
390        o = o - 4;
400        if(o <= 0) {
410            o = 143; s = s + 1;
420            gcursor(0, 0); printf("SCORE:%d", s);
430        }
440        /* 450行目修正：新しい柱を描画 (reverse=0:セット, mask=0xFFFF, rectangle=0:直線) */
450        line(o, 8, o, 8+h, 0, 0xFFFF, 0); 
460        
470        /* 当たり判定 (簡易) */
480        if(o >= x && o <= x+7 && y < 8+h) break;
490    }
500    clscr();
510    printf("GAME OVER\n");
520    printf("SCORE:%d\n", s);
530    printf("PRESS 1 TO RESTART\n");
540    while(1) { if(kbhit() && getch() == '1') break; }
550    goto 120;
560 }