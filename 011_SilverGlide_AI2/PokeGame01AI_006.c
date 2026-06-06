10 unsigned long r_seed = 2463534242;
20 int xorshift() {
30     r_seed ^= r_seed << 13;
40     r_seed ^= r_seed >> 17;
50     r_seed ^= r_seed << 5;
60     return (int)(r_seed % 32768);
70 }
80 main() {
90     int x, y, s, o, h, t, w;
100    int keybit;
110    clrscr();
120    printf("SILVER GLIDE 1\n");
130    printf("PRESS 1 KEY TO START\n");
140    while(1) {
150        outport(0x11, 0x20);
160        keybit = inport(0x10);
170        if((keybit & 0x08) != 0) break;
180        r_seed++; /* キーを待つ間にシード値を回して完全ランダム化 */
190    }
200 RESTART:
210    clrscr(); /* 【対策】リスタート時に前回の残像を完全クリア */
220    x = 20; y = 24; s = 0; o = 143; h = 16; t = 0;
230    gotoxy(0, 0); printf("SCORE:%d", s);
240    while(1) {
250        /* 自機消去 */
260        gcursor(x, y);
270        gprint("0000000000000000");
280        
290        /* キー入力と自機制御（移動量を適度に小さく変更） */
300        outport(0x11, 0x20);
310        keybit = inport(0x10);
320        if((keybit & 0x08) != 0) {
330            y = y - 2; /* 上昇量を 4->2 にマイルド化 */
340            r_seed += y; /* 乱数のブレを大きくする */
350        } else {
360            y = y + 1; /* 落下量を 2->1 にマイルド化 */
370        }
380        if(y < 8) y = 8;
390        if(y > 47) break; 
400        
410        /* 自機描画 */
420        gcursor(x, y);
430        gprint("4040F0F0E0E08000");
440        
450        /* 柱の制御と描画（1ドット単位移動に強化） */
460        line(o, 8, o, 47, 1, 0xFFFF, 0); /* 1ドット単位で古い柱を消去 */
470        o = o - 1; /* 【対策】4ドットから1ドット移動へ */
480        if(o <= 0) {
490            o = 143;
510            /* 自作乱数で柱を決定：高さ8,16,24 / 上下タイプ0,1 */
520            h = ((xorshift() % 3) + 1) * 8; 
530            t = xorshift() % 2; 
540            s = s + 1;
550            gotoxy(0, 0); printf("SCORE:%d", s);
560        }
570        
580        /* 柱の描画 */
590        if(t == 0) {
600            line(o, 8, o, 8+h, 0, 0xFFFF, 0);
610        } else {
620            line(o, 47-(h-4), o, 47, 0, 0xFFFF, 0);
630        }
640        
650        /* 当たり判定 */
660        if(o >= x && o <= x+7) {
670            if(t == 0 && y < 8+h) break;
680            if(t == 1 && y > 47-(h-4)-7) break;
690        }
700        
710        /* 【対策】爆速化防止のウェイトループ */
720        /* 早すぎる場合は 200 を大きく、遅すぎる場合は小さく調整してください */
730        for(w = 0; w < 10; w++); 
740    }
750    clrscr(); /* 【対策】ゲームオーバー画面移行時も画面をクリア */
760    printf("GAME OVER\n");
770    printf("SCORE:%d\n", s);
780    printf("PRESS 1 TO RESTART\n");
790    while(1) { 
800        outport(0x11, 0x20);
810        keybit = inport(0x10);
820        if((keybit & 0x08) != 0) break;
830    }
840    goto RESTART;
850 }