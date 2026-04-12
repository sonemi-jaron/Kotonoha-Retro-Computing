; ======================================================
; LCD chip force-on & Infinite loop test
; ======================================================

LCD_CT		EQU	040h
LCD_NM		EQU	041h

		ORG	0100h		; 入力パラメータ
IN_X:		DB	0		; 100:X
IN_P:		DB	1		; 101:Page(Y/8)
IN_W:		DB	1		; 102:W

		ORG	0110h
START:
		;LD	A, 025h		; 表示ON
		;OUT	(LCD_CT), A

		LD	A, 040h		; スタートラインを0(最上段)に固定
		OUT	(LCD_CT), A

		;LD	A, 0B0h		; Page 0
		LD	A, (IN_P)
		ADD	A, 0B0h
		OUT	(LCD_CT), A

		;LD	A, 000h		; X=0（下位）
		LD	A, (IN_X)
		AND	00Fh
		ADD	A, 000h
		OUT	(LCD_CT), A

		;LD	A, 010h		;    （上位）
		LD	A, (IN_X)
		RRA
		RRA
		RRA
		RRA
		AND	00Fh
		ADD	A, 010h
		OUT	(LCD_CT), A

		LD	A, (IN_W)	; Ｗ（転送幅）
		LD	B, A

FILL:		LD	A, (HL)
		INC	HL
		SUB	030h
		CP	00Ah
		JP	C, FILL_2
		SUB	7
FILL_2:		RLA
		RLA
		RLA
		RLA
		AND	0F0h
		LD	C, A		; １文字目から上位４ビットを作成、Cレジスタに。

		LD	A, (HL)
		INC	HL
		SUB	030h
		CP	00Ah
		JP	C, FILL_3
		SUB	7
FILL_3:		AND	00Fh		; ２文字から下位４ビットを作成、Aレジスタに。

		OR	C		; 上位と下位ビットを、Aレジスタに融合。
		OUT	(LCD_NM), A	; I/Oへデータ出力
		DJNZ	FILL

		RET
