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
125 RESTART:
130    x = 20; y = 24; s = 0; o = 143; h = 16; t = 0;
140    gcursor(0, 0); printf("SCORE:%d", s);
150    while(1) {
160        /* 自機消去 (4隅のドットを消去) */
170        preset(x, y); preset(x+7, y); preset(x, y+7); preset(x+7, y+7);
180        
190        /* 入力処理 */
200        if(kbhit()) {
210            k = getch();
220            if(k == '1') y = y - 4;
230            else y = y + 2;
240        } else {
250            y = y + 2;
260        }
270        if(y < 8) y = 8;
280        if(y > 47) break; /* GAME OVERへ */
290        
300        /* 自機描画 (ドット絵) */
310        gcursor(x, y);
320        gprint("4040F0F0E0E08000");
330        
340        /* 柱の制御と描画（BASIC版ロジック完全移植版） */
350        line(o, 8, o, 47, 1, 0xFFFF, 0); /* 前回の柱全体を消去 */
360        o = o - 4;
370        if(o <= 0) {
380            o = 143; 
390            /* C言語には標準のrndがない、またはBASICと仕様が違う可能性を考慮し、暫定で固定値化 */
400            /* 実機で動いたら後で乱数関数（rand等）の仕様に合わせて修正します */
410            h = 16; /* ひとまず固定の高さ */
420            t = 0;  /* ひとまず上空タイプ固定 */
430            s = s + 1;
440            gcursor(0, 0); printf("SCORE:%d", s);
450        }
460        
470        /* 柱の描画（t=0:上空から、t=1:地面から） */
480        if(t == 0) {
490            line(o, 8, o, 8+h, 0, 0xFFFF, 0);
500        } else {
510            line(o, 47-(h-4), o, 47, 0, 0xFFFF, 0);
520        }
530        
540        /* 当たり判定 (BASIC版準拠) */
550        if(o >= x && o <= x+7) {
560            if(t == 0 && y < 8+h) break;
570            if(t == 1 && y > 47-(h-4)-7) break;
580        }
590    }
600    clscr();
610    printf("GAME OVER\n");
620    printf("SCORE:%d\n", s);
630    printf("PRESS 1 TO RESTART\n");
640    while(1) { if(kbhit() && getch() == '1') break; }
650    goto RESTART; /* 550行目のエラー原因をラベル指定に修正 */
660 }