
    .include        "apple2.inc"
    .include        "zeropage.inc"

    .import         _memcpy
    .import         popa
    .import         popax
    .import         pusha
    .import         pushax
    .import         _gfx_pixel
    .import         _get_color

    .export _init_asm
    .export _count_neighbours
    .export _update_wip


    .define NB_LINES    40
    .define NB_COLUMNS  40
    .define JUMP_BEGINNING_NEXT_LINE    NB_LINES - 2
    .define ALIVE 1
    .define DEAD  0
    .define BLACK 0
    .define CELLS_SIZE 1600


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
    STA tmp4
    TYA
    ADC #JUMP_BEGINNING_NEXT_LINE
    TAY
    LDA tmp4
    ADC (cell),Y
    INY
    INY
    ADC (cell),Y

    ;next row
    STA tmp4
    TYA
    ADC #JUMP_BEGINNING_NEXT_LINE
    TAY
    LDA tmp4
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
nb_neighbours           := tmp1

    LDA ptr1
    LDX ptr1+1
    JSR pushax
    LDA ptr2
    LDX ptr2+1
    JSR pushax
    LDA ptr3
    LDX ptr3+1
    JSR pushax

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
    ;those macros don't touch Y
    copy_16 cell_curr, cell_line
    copy_16 cell_neighbourhoud_line, cell_neighbourhoud
    copy_16 cell_future_line, cell_future
    STY tmp3
    CPY #NB_LINES-1
    BEQ end_y

    ; loop : for( x = 1u; x < NB_COLUMNS - 1u; ++x )
    LDX #1
loop_x:
    CPX #NB_COLUMNS-1
      BEQ end_x

      ; uint8_t nb_neighbours = count_neighbours( cell_neighbourhoud_line )
      STX tmp2
      LDA cell_neighbourhoud_line
      LDX cell_neighbourhoud_line+1
      JSR _count_neighbours  ; nb_neighbours in A
      STA nb_neighbours
      JMP tst_alive

next_x:
    add_16 cell_curr, cell_curr, #NB_LINES, #0
    add_16 cell_neighbourhoud_line, cell_neighbourhoud_line, #NB_LINES, #0
    add_16 cell_future_line, cell_future_line, #NB_LINES, #0
    ; next X
    LDX tmp2
    INX
    JMP loop_x
end_x:
    add_16 cell_line, cell_line, #1, #0
    add_16 cell_neighbourhoud, cell_neighbourhoud, #1, #0
    add_16 cell_future, cell_future, #1, #0
    ; next Y
    LDY tmp3
    INY
    JMP loop_y
end_y:

    JMP end_update  ; This jump allows conditional branching
                    ; tst_dead section in the following (optimization)

; *** TIP */
; those should be placed inside loop_x (search for JMP tst_alive)
; they are placed here not to overflow the 8 bit range of conditional branching
tst_dead:
      LDY #0
      LDA (cell_curr),Y
      CMP #DEAD
      BNE next_x
        LDA nb_neighbours
        CMP #3
        BNE next_x
          ; a cell is born
          LDA #ALIVE
          STA (cell_future_line),Y
          JSR _get_color  ;random color in A
          JSR pusha
          LDA tmp2
          JSR pusha
          LDA tmp3
          JSR _gfx_pixel
          JMP next_x

tst_alive:
      LDY #0
      LDA (cell_curr),Y
      CMP #ALIVE
      BNE tst_dead
        LDA nb_neighbours
        CLC
        ADC #$FE
        BCC kill_cell        ; if nb_neighbours < 2
          LDA nb_neighbours
          CLC
          ADC #$FC            ; no need to CLC again as prev tst failed
          BCC tst_dead        ; if nb_neighbours <= 3
kill_cell:
            LDA #DEAD
            STA (cell_future_line),Y
            JSR pusha           ; #DEAD == #BLACK COLOR
            LDA tmp2
            JSR pusha
            LDA tmp3
            JSR _gfx_pixel
            JMP next_x

end_update:

    ; Cells_Future -> Cells
    LDA Cells
    LDX Cells+1
    JSR pushax
    LDA Cells_Future
    LDX Cells_Future+1
    JSR pushax
    LDA #<CELLS_SIZE
    LDX #>CELLS_SIZE
    JSR _memcpy

    JSR popax
    STA ptr3
    STX ptr3+1
    JSR popax
    STA ptr2
    STX ptr2+1
    JSR popax
    STA ptr1
    STX ptr1+1

    RTS

;*************** END OF UPDATE **************
