; ======================================================
;	仮想ＶＲＡＭ式スプライト描画
; ======================================================

LCD_CT		EQU	040h
LCD_NM		EQU	041h

		ORG	0100h
		JP	MAIN

		ORG	0104h
SPRPRM:		DW	0
		ORG	0106h
SPRPOS:		DW	0

MAIN:
		CALL	CLS
		CALL	SPR
		CALL	DRAW
		RET

; ------------------------------------------------------
;	仮想ＶＲＡＭをクリア
; ------------------------------------------------------
CLS:
		LD	HL, VVRAM
		LD	BC, 1280
		LD	D, 0
CLS_1:
		LD	(HL), D
		INC	HL
		DEC	BC
		LD	A, B
		OR	C
		JP	NZ, CLS_1
		RET

; ------------------------------------------------------
;	仮想ＶＲＡＭをI/Oを使って実描画
; ------------------------------------------------------
DRAW:		
		;LD	A, 025h		; 表示ON
		;OUT	(LCD_CT), A	;
		;LD	A, 040h		; スタートラインを0(最上段)に固定
		;OUT	(LCD_CT), A	;（※不要っぽいので削除）

		LD	HL, VVRAM
		;LD	BC, 168
		LD	BC, 216			;右側96ドット限定処置
		ADD	HL, BC

		LD	D, 0B0h		; Page 0
DRAW_Y:
		LD	A, D		; Page 0～5
		OUT	(LCD_CT), A

		LD	A, 000h		; X=0（下位4bit）
		OUT	(LCD_CT), A
		;LD	A, 010h		;    （上位4bit）
		LD	A, 013h			;右側96ドット限定処置
		OUT	(LCD_CT), A

		;LD	A, 144		; Ｗ（転送幅）
		LD	A, 96			;右側96ドット限定処置
		LD	B, A
DRAW_X:
		LD	A, (HL)
		INC	HL
		OUT	(LCD_NM), A	; I/Oへデータ出力
		DJNZ	DRAW_X
		
		;LD	BC, 16
		LD	BC, 64			;右側96ドット限定処置
		ADD	HL, BC
		
		INC	D
		LD	A, D
		CP	0B6h
		JP	NZ, DRAW_Y

		RET

; ------------------------------------------------------
;	スプライト
; ------------------------------------------------------
SPR:
		LD	B, 32
		LD	IY, (SPRPRM)
		LD	IX, (SPRPOS)
SPR_L:		
		PUSH	BC

		LD	A, (IY+0)
		LD	(SPR_N), A	;sprPrm[n][0] No (uchar)
		LD	A, (IY+1)
		LD	(SPR_W), A	;sprPrm[n][1] W (uchar)
		LD	A, (IY+2)
		LD	(SPR_H), A	;sprPrm[n][2] H (uchar)
		LD	DE, 3
		ADD	IY, DE

		CALL	BCD
		LD	(SPR_X), DE	;sprPos[n][0] X (float)
		LD	DE, 4
		ADD	IX, DE
		CALL	BCD
		LD	(SPR_Y), DE	;sprPos[n][1] Y (float)
		LD	DE, 4
		ADD	IX, DE

		LD	A, (SPR_N)
		CP	0
		JP	Z,SPR_L1
		
		DEC	A
		LD	E, A
		LD	D, 0
		SLA	E
		RL	D
		SLA	E
		RL	D
		SLA	E
		RL	D
		LD	HL, SPRDAT
		ADD	HL, DE		;HL = SPRDAT + (SPR_N-1)*8
		LD	(SPR_D), HL

		LD	HL, (SPR_X)
		LD	(SPR_KX), HL
		LD	A, (SPR_H)
		LD	B, A
SPR_LH:
		PUSH	BC

		LD	A, (SPR_W)
		LD	B, A
SPR_LW:
		PUSH	BC

		CALL	SPR_00
		LD	DE, 8
		LD	HL, (SPR_D)
		ADD	HL, DE
		LD	(SPR_D), HL
		LD	HL, (SPR_X)
		ADD	HL, DE
		LD	(SPR_X), HL

		POP	BC
		DJNZ	SPR_LW

		LD	HL,(SPR_KX)
		LD	(SPR_X), HL
		LD	HL, (SPR_Y)
		ADD	HL, DE
		LD	(SPR_Y), HL

		POP	BC
		DJNZ	SPR_LH

SPR_L1:
		POP	BC
		DEC	B
		JP	NZ, SPR_L
		RET

; ------------------------------------------------------
SPR_00:
		PUSH	IX
		PUSH	IY
		
		; X座標ＶＲＡＭ外判定
		LD	HL,(SPR_X)
		LD	DE, 8
		ADD	HL, DE
		BIT	7, H
		JP	NZ, SPR_99

		LD	DE, -160
		ADD	HL, DE
		BIT	7, H
		JP	Z, SPR_99

		LD	DE, 160
		ADD	HL, DE
		LD	(SPR_W0), HL

		; Y座標ＶＲＡＭ外判定
		LD	HL, (SPR_Y)
		LD	DE, 9
		ADD	HL, DE
		BIT	7, H
		JP	NZ, SPR_99

		LD	DE, -64
		ADD	HL, DE
		BIT	7, H
		JP	Z, SPR_99

		LD	DE, 55
		ADD	HL, DE
		LD	(SPR_W1), HL
		
		SRA	H	; HL / 8
		RR	L
		SRA	H
		RR	L
		SRA	H
		RR	L
		LD	A, L
		LD	HL, (SPR_W0)
		CP	0
		JP	Z, SPR_02

		LD	B, A
		LD	DE, 160
SPR_01:
		ADD	HL, DE
		DJNZ	SPR_01
SPR_02:
		LD	DE, (SPR_W1)
		LD	A, E
		AND	7
		CP	7
		JP	NZ, SPR_10

		; 8x8のスプライトＶＲＡＭに描画（Yが８の倍数位置専用）
		LD	DE, VVRAM
		ADD	HL, DE
		LD	DE, 160
		ADD	HL, DE
		LD	DE, (SPR_D)
		LD	B, 8
SPR_05:
		LD	A, (DE)
		LD	C, (HL)
		OR	C
		LD	(HL), A
		INC	DE
		INC	HL
		DJNZ	SPR_05
		JP	SPR_99
SPR_10:
		; 8x8のスプライトＶＲＡＭに描画
		INC	A
		LD	(SPR_B0), A
		LD	DE, VVRAM
		ADD	HL, DE
		LD	IY, (SPR_D)
		LD	C, 0
SPR_11:
		LD	D, (IY+0)
		LD	E, 0
		INC	IY
		LD	A, (SPR_B0)
		LD	B, A
SPR_12:
		SLA	D
		RL	E
		DJNZ	SPR_12

		LD	A, D
		LD	(SPR_B1), A
		LD	A, E
		LD	(SPR_B2), A

		LD	A, (SPR_B1)
		LD	D, A
		LD	A, (HL)
		OR	D
		LD	(HL), A
		LD	DE, 160
		ADD	HL, DE
		LD	A, (SPR_B2)
		LD	D, A
		LD	A, (HL)
		OR	D
		LD	(HL), A
		LD	DE,-159
		ADD	HL, DE
	
		INC	C
		LD	A,C
		CP	8
		JP	NZ, SPR_11
SPR_99:	
		POP	IY
		POP	IX
		RET

; ------------------------------------------------------
;	ＢＣＤ形式を数値に（3桁まで）
; ------------------------------------------------------
BCD:
		LD	A, (IX+1)
		AND	0F0h
		CP	020h
		JP	Z, BCD_30
		CP	010h
		JP	Z, BCD_20
		
		LD	A, (IX+2)
		RRCA
		RRCA
		RRCA
		RRCA
		AND	00Fh
		LD	D, 0
		LD	E, A
		JP	BCD_50
BCD_20:
		LD	A, (IX+2)
		RRCA
		RRCA
		RRCA
		RRCA
		AND	00Fh
		LD	C, A
		ADD	A, A
		ADD	A, A
		ADD	A, C
		ADD	A, A
		LD	C, A
		LD	A, (IX+2)
		AND	00Fh
		ADD	A, C
		LD	D, 0
		LD	E, A
		JP	BCD_50
BCD_30:
		LD	A, (IX+3)
		RRCA
		RRCA
		RRCA
		RRCA
		AND	00Fh
		LD	L, A
		LD	A, (IX+2)
		AND	00Fh
		LD	C, A
		ADD	A, A
		ADD	A, A
		ADD	A, C
		ADD	A, A
		ADD	A, L
		LD	H, 0
		LD	L, A
		LD	A, (IX+2)
		RRCA
		RRCA
		RRCA
		RRCA
		AND	00Fh
		LD	B, A
		LD	DE, 100
BCD_31:
		ADD	HL, DE
		DJNZ	BCD_31
		
		LD	D, H
		LD	E, L
BCD_50:
		LD	A, (IX+1)
		AND	008h
		CP	0
		JP	Z, BCD_90
		
		LD	A, D
		CPL
		LD	D, A
		LD	A, E
		CPL
		LD	E, A
		INC	DE
BCD_90:
		RET
		
; ------------------------------------------------------
;	変数
; ------------------------------------------------------
SPR_N:		DB	0
SPR_W:		DB	0
SPR_H:		DB	0
SPR_B0:		DB	0
SPR_B1:		DB	0
SPR_B2:		DB	0

SPR_D:		DW	0
SPR_X:		DW	0
SPR_Y:		DW	0
SPR_KX:		DW	0
SPR_W0:		DW	0
SPR_W1:		DW	0

; ------------------------------------------------------
;	スプライトデータ（８ｘ８dot固定）
; ------------------------------------------------------
SPRDAT:		DB	057h,07Eh,07Ch,05Ch,04Ch,00Ch,00Ch,008h	;1:自機	
		DB	053h,096h,072h,059h,05Ch,08Ch,02Eh,009h	;2:爆発
		DB	000h,000h,081h,0C3h,07Eh,03Ch,000h,000h	;3:ショット
		DB	000h,018h,024h,05Ah,05Ah,024h,018h,000h	;4:敵弾
		DB	08Ch,052h,076h,07Eh,03Ch,06Eh,061h,040h	;5:敵１
		DB	060h,0D2h,0D3h,0F3h,0F3h,07Fh,07Eh,038h	;6:敵２
		DB	040h,0C8h,0ECh,0F6h,0F7h,0FFh,0FEh,078h	;7:敵３
		DB	0FFh,081h,0FFh,0FFh,081h,0FFh,081h,0FFh	;8:柱
		DB	0FFh,081h,0FFh,0FFh,081h,0FFh,081h,0FFh	;9:柱

		DB	080h,0C0h,040h,0C0h,0C0h,0C0h,0C0h,0C0h	;10:ボス
		DB	0C0h,0E0h,0F0h,0F8h,0FCh,0FCh,0CCh,0CCh	;11
		DB	0C8h,0E0h,0F0h,030h,010h,080h,0C0h,0C0h	;12
		DB	003h,007h,00Fh,00Ah,00Fh,00Fh,00Fh,01Fh	;13
		DB	01Fh,03Fh,00Fh,00Fh,00Fh,00Fh,00Fh,00Bh	;14
		DB	01Dh,03Fh,03Fh,02Fh,027h,02Dh,00Fh,00Ah	;15

; ------------------------------------------------------
;	仮想ＶＲＡＭ
; ------------------------------------------------------
VVRAM:		DW	0	; 実体無し
;VVRAM:		DS	1280	; 実体有り
