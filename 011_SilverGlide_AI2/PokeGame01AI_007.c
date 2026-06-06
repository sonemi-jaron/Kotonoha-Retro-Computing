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
180        r_seed++; 
190    }
200 RESTART:
210    clrscr(); 
220    /* yの初期値を変更：左下基準なので、初期位置y=32（上端が25）付近に配置 */
230    x = 20; y = 32; s = 0; o = 143; h = 16; t = 0;
240    gotoxy(0, 0); printf("SCORE:%d", s);
250    while(1) {
260        /* 自機消去（左下基準で消去データを入れる） */
270        gcursor(x, y);
280        gprint("0000000000000000");
290        
300        /* キー入力と自機制御 */
310        outport(0x11, 0x20);
320        keybit = inport(0x10);
330        if((keybit & 0x08) != 0) {
340            y = y - 2; 
350            r_seed += y; 
360        } else {
370            y = y + 1; 
380        }
390        
400        /* 【対策】移動範囲と死亡判定の修正（左下基準） */
410        /* ドット絵の上端(y-7)がスコア行(7以下)にめり込まないよう、yの下限を15に制限 */
420        if(y < 15) y = 15;
430        /* ドット絵の左下(y)が画面最下部(47)を超えたら死亡判定 */
440        if(y > 47) break; 
450        
460        /* 自機描画（左下基準で描画される） */
470        gcursor(x, y);
480        gprint("4040F0F0E0E08000");
490        
500        /* 柱の制御と描画 */
510        line(o, 8, o, 47, 1, 0xFFFF, 0); 
520        o = o - 1; 
530        if(o <= 0) {
540            o = 143;
550            h = ((xorshift() % 3) + 1) * 8; 
560            t = xorshift() % 2; 
570            s = s + 1;
580            gotoxy(0, 0); printf("SCORE:%d", s);
590        }
600        
610        /* 柱の描画 */
620        if(t == 0) {
630            line(o, 8, o, 8+h, 0, 0xFFFF, 0);
640        } else {
650            line(o, 47-(h-4), o, 47, 0, 0xFFFF, 0);
660        }
670        
680        /* 【対策】当たり判定の修正（自機の上端が y-7、下端が y であることを考慮） */
690        if(o >= x && o <= x+7) {
700            /* t=0(天井からの柱)：柱の下端(8+h)より、自機の上端(y-7)が上に行ったら衝突 */
710            if(t == 0 && (y - 7) < (8 + h)) break;
720            /* t=1(地面からの柱)：柱の上端(47-(h-4))より、自機の下端(y)が下に行ったら衝突 */
730            if(t == 1 && y > (47 - (h - 4))) break;
740        }
750        
760        /* 【対策】局長環境に合わせた最適なウェイト値（25に設定） */
770        for(w = 0; w < 25; w++); 
780    }
790    clrscr(); 
800    printf("GAME OVER\n");
810    printf("SCORE:%d\n", s);
820    printf("PRESS 1 TO RESTART\n");
830    while(1) { 
840        outport(0x11, 0x20);
850        keybit = inport(0x10);
860        if((keybit & 0x08) != 0) break;
870    }
880    goto RESTART;
890 }