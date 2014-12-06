
	INCLUDE	"hardware.inc"
	INCLUDE "header.inc"

;--------------------------------------------------------------------------

	SECTION	"Main",HOME

;------------------------------------

PrepareSprites: ; d = number of sprites in test line
	
	ld	b,144
	call	wait_ly
	
	ld	hl,$FE00
.loop:
	ld	a,d
	and	a,a
	ret	z
	
	ld	a,48+16
	ld	[hl+],a
	ld	a,50
	ld	[hl+],a
	ld	a,0
	ld	[hl+],a
	ld	[hl+],a
	
	dec	d
	
	jr .loop

;--------------------------------------------------------------------------
;- Main()                                                                 -
;--------------------------------------------------------------------------

Main:

	; -------------------------------------------------------
	
	ld	a,$0A
	ld	[$0000],a ; enable ram
	
	ld	hl,$A000
	
	ld	a,LCDCF_ON|LCDCF_OBJ16
	ld	[rLCDC],a
	
	; -------------------------------------------------------

PERFORM_TEST : MACRO
	di
	
	push	hl
	
	ld	bc,$007F
	ld	hl,\1
	ld	de,$FF80
	call	memcopy
	
	ld	b,45
	call	wait_ly
	
	ld	a,50
	ld	[rLYC],a
	ld	a,STATF_LYC
	ld	[rSTAT],a
	
	ld	a,IEF_LCDC
	ld	[rIE],a
	
	xor	a,a
	ld	[rIF],a
	
	pop	hl
	
	ei

	halt
ENDM
	
	ld	d,0
.next_spr_number:
	push	de
	push	hl
	call	PrepareSprites
	pop		hl
	
	ld	a,$80
	ld	[rNR52],a
	ld	a,$FF
	ld	[rNR51],a
	ld	a,$77
	ld	[rNR50],a
	
	ld	a,$C0
	ld	[rNR11],a
	ld	a,$E0
	ld	[rNR12],a
	ld	a,$00
	ld	[rNR13],a
	ld	a,$82
	ld	[rNR14],a
	
	PERFORM_TEST LCD_INT_HANDLER_0
	PERFORM_TEST LCD_INT_HANDLER_1
	PERFORM_TEST LCD_INT_HANDLER_2
	PERFORM_TEST LCD_INT_HANDLER_3
	PERFORM_TEST LCD_INT_HANDLER_4
	PERFORM_TEST LCD_INT_HANDLER_5
	PERFORM_TEST LCD_INT_HANDLER_6
	PERFORM_TEST LCD_INT_HANDLER_7
	PERFORM_TEST LCD_INT_HANDLER_8
	PERFORM_TEST LCD_INT_HANDLER_9
	PERFORM_TEST LCD_INT_HANDLER_10
	PERFORM_TEST LCD_INT_HANDLER_11
	PERFORM_TEST LCD_INT_HANDLER_12
	PERFORM_TEST LCD_INT_HANDLER_13
	PERFORM_TEST LCD_INT_HANDLER_14
	PERFORM_TEST LCD_INT_HANDLER_15
	
	ld	a,$80
	ld	[rNR52],a
	ld	a,$FF
	ld	[rNR51],a
	ld	a,$77
	ld	[rNR50],a
	
	ld	a,$C0
	ld	[rNR11],a
	ld	a,$E0
	ld	[rNR12],a
	ld	a,$00
	ld	[rNR13],a
	ld	a,$83
	ld	[rNR14],a
	
	PERFORM_TEST LCD_INT_HANDLER_16
	PERFORM_TEST LCD_INT_HANDLER_17
	PERFORM_TEST LCD_INT_HANDLER_18
	PERFORM_TEST LCD_INT_HANDLER_19
	PERFORM_TEST LCD_INT_HANDLER_20
	PERFORM_TEST LCD_INT_HANDLER_21
	PERFORM_TEST LCD_INT_HANDLER_22
	PERFORM_TEST LCD_INT_HANDLER_23
	PERFORM_TEST LCD_INT_HANDLER_24
	PERFORM_TEST LCD_INT_HANDLER_25
	PERFORM_TEST LCD_INT_HANDLER_26
	PERFORM_TEST LCD_INT_HANDLER_27
	PERFORM_TEST LCD_INT_HANDLER_28
	PERFORM_TEST LCD_INT_HANDLER_29
	PERFORM_TEST LCD_INT_HANDLER_30
	PERFORM_TEST LCD_INT_HANDLER_31
	
	ld	a,$80
	ld	[rNR52],a
	ld	a,$FF
	ld	[rNR51],a
	ld	a,$77
	ld	[rNR50],a
	
	ld	a,$C0
	ld	[rNR11],a
	ld	a,$E0
	ld	[rNR12],a
	ld	a,$00
	ld	[rNR13],a
	ld	a,$82
	ld	[rNR14],a
	
	pop	de
	inc	d
	ld	a,16
	cp	a,d
	jp	nz,.next_spr_number
	
	; --------------------------------
	
	ld	a,$80
	ld	[rNR52],a
	ld	a,$FF
	ld	[rNR51],a
	ld	a,$77
	ld	[rNR50],a
	
	ld	a,$C0
	ld	[rNR11],a
	ld	a,$E0
	ld	[rNR12],a
	ld	a,$00
	ld	[rNR13],a
	ld	a,$87
	ld	[rNR14],a
	
	push	hl
	ld	[hl],$12
	inc hl
	ld	[hl],$34
	inc hl
	ld	[hl],$56
	inc hl
	ld	[hl],$78
	pop	hl
	
	ld	a,$00
	ld	[$0000],a ; disable ram
	
	call	CPU_fast
	
	ld	a,$0A
	ld	[$0000],a ; enable ram
	
	; ---------------------------------

	ld	d,0
.next_spr_number2:
	push	de
	push	hl
	call	PrepareSprites
	pop		hl
	
	ld	a,$80
	ld	[rNR52],a
	ld	a,$FF
	ld	[rNR51],a
	ld	a,$77
	ld	[rNR50],a
	
	ld	a,$C0
	ld	[rNR11],a
	ld	a,$E0
	ld	[rNR12],a
	ld	a,$00
	ld	[rNR13],a
	ld	a,$82
	ld	[rNR14],a
	
	PERFORM_TEST LCD_INT_HANDLER_GBC_0
	PERFORM_TEST LCD_INT_HANDLER_GBC_1
	PERFORM_TEST LCD_INT_HANDLER_GBC_2
	PERFORM_TEST LCD_INT_HANDLER_GBC_3
	PERFORM_TEST LCD_INT_HANDLER_GBC_4
	PERFORM_TEST LCD_INT_HANDLER_GBC_5
	PERFORM_TEST LCD_INT_HANDLER_GBC_6
	PERFORM_TEST LCD_INT_HANDLER_GBC_7
	PERFORM_TEST LCD_INT_HANDLER_GBC_8
	PERFORM_TEST LCD_INT_HANDLER_GBC_9
	PERFORM_TEST LCD_INT_HANDLER_GBC_10
	PERFORM_TEST LCD_INT_HANDLER_GBC_11
	PERFORM_TEST LCD_INT_HANDLER_GBC_12
	PERFORM_TEST LCD_INT_HANDLER_GBC_13
	PERFORM_TEST LCD_INT_HANDLER_GBC_14
	PERFORM_TEST LCD_INT_HANDLER_GBC_15
	PERFORM_TEST LCD_INT_HANDLER_GBC_16
	
	ld	a,$80
	ld	[rNR52],a
	ld	a,$FF
	ld	[rNR51],a
	ld	a,$77
	ld	[rNR50],a
	
	ld	a,$C0
	ld	[rNR11],a
	ld	a,$E0
	ld	[rNR12],a
	ld	a,$00
	ld	[rNR13],a
	ld	a,$83
	ld	[rNR14],a
	
	PERFORM_TEST LCD_INT_HANDLER_GBC_17
	PERFORM_TEST LCD_INT_HANDLER_GBC_18
	PERFORM_TEST LCD_INT_HANDLER_GBC_19
	PERFORM_TEST LCD_INT_HANDLER_GBC_20
	PERFORM_TEST LCD_INT_HANDLER_GBC_21
	PERFORM_TEST LCD_INT_HANDLER_GBC_22
	PERFORM_TEST LCD_INT_HANDLER_GBC_23
	PERFORM_TEST LCD_INT_HANDLER_GBC_24
	PERFORM_TEST LCD_INT_HANDLER_GBC_25
	PERFORM_TEST LCD_INT_HANDLER_GBC_26
	PERFORM_TEST LCD_INT_HANDLER_GBC_27
	PERFORM_TEST LCD_INT_HANDLER_GBC_28
	PERFORM_TEST LCD_INT_HANDLER_GBC_29
	PERFORM_TEST LCD_INT_HANDLER_GBC_30
	PERFORM_TEST LCD_INT_HANDLER_GBC_31
	
	pop	de
	inc	d
	ld	a,16
	cp	a,d
	jp	nz,.next_spr_number2
	

	; -------------------------------------------------------
	
	ld	a,$80
	ld	[rNR52],a
	ld	a,$FF
	ld	[rNR51],a
	ld	a,$77
	ld	[rNR50],a
	
	ld	a,$C0
	ld	[rNR11],a
	ld	a,$E0
	ld	[rNR12],a
	ld	a,$00
	ld	[rNR13],a
	ld	a,$87
	ld	[rNR14],a
	
	push	hl
	ld	[hl],$12
	inc hl
	ld	[hl],$34
	inc hl
	ld	[hl],$56
	inc hl
	ld	[hl],$78
	pop	hl
	
	ld	a,$00
	ld	[$0000],a ; disable ram

.endloop:
	halt
	jr	.endloop

; --------------------------------------------------------------

	SECTION "functions",ROMX,BANK[1]
	
LCD_INT_HANDLER_MACRO : MACRO
	REPT \1
	nop
	ENDR
	ld	a,[rSTAT]
	ld	[hl+],a
	ret
ENDM

LCD_INT_HANDLER_0: LCD_INT_HANDLER_MACRO 0
LCD_INT_HANDLER_1: LCD_INT_HANDLER_MACRO 1
LCD_INT_HANDLER_2: LCD_INT_HANDLER_MACRO 2
LCD_INT_HANDLER_3: LCD_INT_HANDLER_MACRO 3
LCD_INT_HANDLER_4: LCD_INT_HANDLER_MACRO 4
LCD_INT_HANDLER_5: LCD_INT_HANDLER_MACRO 5
LCD_INT_HANDLER_6: LCD_INT_HANDLER_MACRO 6
LCD_INT_HANDLER_7: LCD_INT_HANDLER_MACRO 7
LCD_INT_HANDLER_8: LCD_INT_HANDLER_MACRO 8
LCD_INT_HANDLER_9: LCD_INT_HANDLER_MACRO 9
LCD_INT_HANDLER_10: LCD_INT_HANDLER_MACRO 10
LCD_INT_HANDLER_11: LCD_INT_HANDLER_MACRO 11
LCD_INT_HANDLER_12: LCD_INT_HANDLER_MACRO 12
LCD_INT_HANDLER_13: LCD_INT_HANDLER_MACRO 13
LCD_INT_HANDLER_14: LCD_INT_HANDLER_MACRO 14
LCD_INT_HANDLER_15: LCD_INT_HANDLER_MACRO 15
LCD_INT_HANDLER_16: LCD_INT_HANDLER_MACRO 16
LCD_INT_HANDLER_17: LCD_INT_HANDLER_MACRO 17
LCD_INT_HANDLER_18: LCD_INT_HANDLER_MACRO 18
LCD_INT_HANDLER_19: LCD_INT_HANDLER_MACRO 19
LCD_INT_HANDLER_20: LCD_INT_HANDLER_MACRO 20
LCD_INT_HANDLER_21: LCD_INT_HANDLER_MACRO 21
LCD_INT_HANDLER_22: LCD_INT_HANDLER_MACRO 22
LCD_INT_HANDLER_23: LCD_INT_HANDLER_MACRO 23
LCD_INT_HANDLER_24: LCD_INT_HANDLER_MACRO 24
LCD_INT_HANDLER_25: LCD_INT_HANDLER_MACRO 25
LCD_INT_HANDLER_26: LCD_INT_HANDLER_MACRO 26
LCD_INT_HANDLER_27: LCD_INT_HANDLER_MACRO 27
LCD_INT_HANDLER_28: LCD_INT_HANDLER_MACRO 28
LCD_INT_HANDLER_29: LCD_INT_HANDLER_MACRO 29
LCD_INT_HANDLER_30: LCD_INT_HANDLER_MACRO 30
LCD_INT_HANDLER_31: LCD_INT_HANDLER_MACRO 31
	
LCD_INT_HANDLER_GBC_MACRO : MACRO
	push	de
	pop		de
	push	de
	pop		de
	push	de
	pop		de ; 21
	nop
	nop
	nop
	REPT \1
	nop
	ENDR
	ld	a,[rSTAT]
	ld	[hl+],a
	ret
ENDM

LCD_INT_HANDLER_GBC_0: LCD_INT_HANDLER_GBC_MACRO 0
LCD_INT_HANDLER_GBC_1: LCD_INT_HANDLER_GBC_MACRO 1
LCD_INT_HANDLER_GBC_2: LCD_INT_HANDLER_GBC_MACRO 2
LCD_INT_HANDLER_GBC_3: LCD_INT_HANDLER_GBC_MACRO 3
LCD_INT_HANDLER_GBC_4: LCD_INT_HANDLER_GBC_MACRO 4
LCD_INT_HANDLER_GBC_5: LCD_INT_HANDLER_GBC_MACRO 5
LCD_INT_HANDLER_GBC_6: LCD_INT_HANDLER_GBC_MACRO 6
LCD_INT_HANDLER_GBC_7: LCD_INT_HANDLER_GBC_MACRO 7
LCD_INT_HANDLER_GBC_8: LCD_INT_HANDLER_GBC_MACRO 8
LCD_INT_HANDLER_GBC_9: LCD_INT_HANDLER_GBC_MACRO 9
LCD_INT_HANDLER_GBC_10: LCD_INT_HANDLER_GBC_MACRO 10
LCD_INT_HANDLER_GBC_11: LCD_INT_HANDLER_GBC_MACRO 11
LCD_INT_HANDLER_GBC_12: LCD_INT_HANDLER_GBC_MACRO 12
LCD_INT_HANDLER_GBC_13: LCD_INT_HANDLER_GBC_MACRO 13
LCD_INT_HANDLER_GBC_14: LCD_INT_HANDLER_GBC_MACRO 14
LCD_INT_HANDLER_GBC_15: LCD_INT_HANDLER_GBC_MACRO 15
LCD_INT_HANDLER_GBC_16: LCD_INT_HANDLER_GBC_MACRO 16
LCD_INT_HANDLER_GBC_17: LCD_INT_HANDLER_GBC_MACRO 17
LCD_INT_HANDLER_GBC_18: LCD_INT_HANDLER_GBC_MACRO 18
LCD_INT_HANDLER_GBC_19: LCD_INT_HANDLER_GBC_MACRO 19
LCD_INT_HANDLER_GBC_20: LCD_INT_HANDLER_GBC_MACRO 20
LCD_INT_HANDLER_GBC_21: LCD_INT_HANDLER_GBC_MACRO 21
LCD_INT_HANDLER_GBC_22: LCD_INT_HANDLER_GBC_MACRO 22
LCD_INT_HANDLER_GBC_23: LCD_INT_HANDLER_GBC_MACRO 23
LCD_INT_HANDLER_GBC_24: LCD_INT_HANDLER_GBC_MACRO 24
LCD_INT_HANDLER_GBC_25: LCD_INT_HANDLER_GBC_MACRO 25
LCD_INT_HANDLER_GBC_26: LCD_INT_HANDLER_GBC_MACRO 26
LCD_INT_HANDLER_GBC_27: LCD_INT_HANDLER_GBC_MACRO 27
LCD_INT_HANDLER_GBC_28: LCD_INT_HANDLER_GBC_MACRO 28
LCD_INT_HANDLER_GBC_29: LCD_INT_HANDLER_GBC_MACRO 29
LCD_INT_HANDLER_GBC_30: LCD_INT_HANDLER_GBC_MACRO 30
LCD_INT_HANDLER_GBC_31: LCD_INT_HANDLER_GBC_MACRO 31

; --------------------------------------------------------------
