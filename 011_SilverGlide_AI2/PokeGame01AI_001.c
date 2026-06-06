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
160        /* 自機消去 (8x8の矩形をpresetで消去するか、前回の座標を空白化) */
170        /* 簡易的に点としてクリアするか、gprintで全消去データを重ねる */
180        /* ここではマニュアル239Pのpresetを使用 */
190        preset(x, y); preset(x+7, y); preset(x, y+7); preset(x+7, y+7);
200        
210        /* 入力処理 */
220        if(kbhit()) {
230            k = getch();
240            if(k == '1') y = y - 4;
250            else y = y + 2;
260        } else {
270            y = y + 2;
280        }
290        if(y < 8) y = 8;
300        if(y > 47) break; /* GAME OVERへ */
310        
320        /* 自機描画 (ドット絵) */
330        gcursor(x, y);
340        gprint("4040F0F0E0E08000");
350        
360        /* 柱の制御と描画（簡易版。後ほど最適化） */
370        /* マニュアル238Pのline関数を利用：line(x1, y1, x2, y2, reverse, rectangle) */
380        /* reverse=1でリセット(消去)、reverse=0でセット(描画) */
390        line(o, 8, o, 47, 1, 0); /* 前の柱を消去 */
400        o = o - 4;
410        if(o <= 0) {
420            o = 143; s = s + 1;
430            gcursor(0, 0); printf("SCORE:%d", s);
440        }
450        line(o, 8, o, 8+h, 0, 0); /* 新しい柱を描画 */
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