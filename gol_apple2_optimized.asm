
    .include        "apple2.inc"
    .include        "zeropage.inc"
    
    .export _count_neighbours

; ******************
;uint8_t __fastcall__ count_neighbours( uint8_t* cell )
;param: cell is in AX
_count_neighbours:

    ;ASSUMPTIONS:
    ; -> A and Y (offset to starting ptr) won't overflow!

    ;init
    STA ptr1
    STX ptr1+1
    LDA #0
    LDY #0
    CLC
    
    ;acc 1st row
    ADC (ptr1),Y
    INY
    ADC (ptr1),Y
    INY
    ADC (ptr1),Y
    
    ;next row    
    STA tmp1
    TYA
    ADC #$15
    TAY
    LDA tmp1
    ADC (ptr1),Y
    INY
    INY
    ADC (ptr1),Y
    
    ;next row    
    STA tmp1
    TYA
    ADC #$15
    TAY
    LDA tmp1
    ADC (ptr1),Y
    INY
    ADC (ptr1),Y
    INY
    ADC (ptr1),Y
    
    ;return
    TAX
    LDX #0
    RTS
    