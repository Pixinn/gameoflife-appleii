
    .include        "apple2.inc"
    .include        "zeropage.inc"

    .import         popa

    .export _init_asm
    .export _count_neighbours
    .export _update_wip



    .define NB_LINES    40
    .define NB_COLUMNS  40
    .define JUMP_BEGINNING_NEXT_LINE    NB_LINES - 2



; ******************
;uint8_t __fastcall__ count_neighbours( uint8_t* cell )
;param: cell is in AX
; ! A, X, Y & PTR4 ARE OVERWRITTEN !
_count_neighbours:

    ;ASSUMPTIONS:
    ; -> A and Y (offset to starting ptr) won't overflow!

    ;alias
cell := ptr4
    ;init
    STA cell
    STX cell+1
    LDA #0
    LDY #0
    CLC

    ;acc 1st row
    ADC (cell),Y
    INY
    ADC (cell),Y
    INY
    ADC (cell),Y

    ;next row
    STA tmp1
    TYA
    ADC #JUMP_BEGINNING_NEXT_LINE
    TAY
    LDA tmp1
    ADC (cell),Y
    INY
    INY
    ADC (cell),Y

    ;next row
    STA tmp1
    TYA
    ADC #JUMP_BEGINNING_NEXT_LINE
    TAY
    LDA tmp1
    ADC (cell),Y
    INY
    ADC (cell),Y
    INY
    ADC (cell),Y

    ;return
    LDX #0
    RTS





;*************************    WORK IN PROGRESS **************************



    .BSS

Cells:                .word   $0
Cells_Future:         .word   $0

cell_neighbourhoud:   .word   $0
cell_line:            .word   $0
cell_future:          .word   $0


    .CODE

;16 bit addition of the content of an adress with a 16bit value
.macro    add_16  addr_dst, addr_src, value_low, value_hi
  CLC
  LDA addr_src
  ADC value_low
  STA addr_dst
  LDA addr_src+1
  ADC value_hi
  STA addr_dst+1
.endmacro

;16 bit copy from a memory location to another
.macro    copy_16 addr_dst, addr_src
  LDA addr_src
  STA addr_dst
  LDA addr_src+1
  STA addr_dst+1
.endmacro


; ******************
; Initialize the variables used in ASM functions
;void __fastcall__ init_asm( uint8_t* p_cell, uint8_t* p_cells_future )
;param: p_cell is in AX
;       p_cells_future in sreg

_init_asm:

  STA Cells_Future
  STX Cells_Future+1
  LDY #1
  LDA (sp),Y
  STA Cells+1
  DEY
  LDA (sp),Y
  STA Cells

  JSR popa
  JSR popa

  RTS

; ******************
;void __fastcall__ update( void )
; !  A, X, Y & ALL PTRx ARE OVERWRITTEN !
_update_wip:

    ;aliases
cell_curr               := ptr1
cell_neighbourhoud_line := ptr2
cell_future_line        := ptr3

    ;preambule
    ; cell_neighbourhoud = Cells
    copy_16 cell_neighbourhoud, Cells
    ; cell_line = cell_neighbourhoud + NB_LINES + 1u
    add_16 cell_line, cell_neighbourhoud, #NB_LINES+1, #0
    ; cell_future = Cells_Future + NB_LINES + 1u
    add_16 cell_future, Cells_Future, #NB_LINES+1, #0
    ; loop : for( y = 1u; y < NB_LINES - 1u; ++y )
    LDY #1
loop_y:
    CPY #NB_LINES-1
    BEQ end_y
    copy_16 cell_curr, cell_line
    copy_16 cell_neighbourhoud_line, cell_neighbourhoud
    copy_16 cell_future_line, cell_future
    ;PAS BESOIN DE CES POINTEURS: ADRESSE DE BASE + X ???
loop_x:
    CPX #NB_LINES-1
    BEQ end_x

    ;TODO

    INX
    CLC
    BCC loop_x
end_x:
    INY
    CLC
    BCC loop_y
end_y:
    RTS
