; Custom interrupt handler. This version is for Aztec C "as" assembler.

	xdef _rasterpos
	xdef _bytepos
	xdef _int_handler
	xdef _rasterpos2
	xdef _bytepos2

_rasterpos:
	dc.b $00,$00
_bytepos:
	dc.b $00,$00,$00,$00
_splitnum:
	dc.b $00,$00
_rasterpos2:
	dc.b $00,$00
_bytepos2:
	dc.b $00,$00,$00,$00


_int_handler:

delay:
	move.w ($00dff006),d0
	cmp.b #100,d0
	blt delay

	; see which split we're at
	move.b (_splitnum),d0
	bne second_split
	
	move #024,d0	; did i seriously just guess the right delay? 20
	;move #0247,d0 too many scanlines ;)
delay2:
	dbra d0,delay2

	;lea _bytepos,a0
	move.l (_bytepos),($00DFF0e4)	; shift screen by bytes
	

	;lea _rasterpos,a0
	move.w (_rasterpos),d0
;	move.w d0,($00DFF180) 		;test setting background color
	move.w d0,($00DFF102)		; set scroll position
	
	lea _splitnum,a0
	move.w #$ffff,d0
	move.w d0,(a0)
	moveq #0,d0
	rts
	
second_split:

	move #020,d0	; did i seriously just guess the right delay? 20
	;move #0247,d0 too many scanlines ;)
delay3:
	dbra d0,delay3

	;lea _bytepos,a0
	move.l (_bytepos2),($00DFF0e4)	; shift screen by bytes
	

	;lea _rasterpos,a0
	move.w (_rasterpos2),d0
;	move.w d0,($00DFF180) 		;test setting background color
	move.w d0,($00DFF102)		; set scroll position
	
	lea _splitnum,a0
	move.w #$0000,(a0)
	;move.b d0,(_splitnum)
	moveq #0,d0
	rts
