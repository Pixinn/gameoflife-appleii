; Conway's Game of Life for Apple II
; Copyright (C) 2016 Christophe Meneboeuf <christophe@xtof.info>
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program. If not, see <http://www.gnu.org/licenses/>.
;

  .include        "apple2.inc"
  .include        "zeropage.inc"

  .import         pusha
  .import         popa
  .import         pushax
  .import         popax

  .export         _mode_text
  .export         _gfx_init
  .export         _gfx_fill
  .export         _gfx_pixel
  .export         _gfx_get_pixel
  .export         _gfx_refresh


  .define         FULLSCREEN    #0
  .define         SPLIT         #1
  .define         LOWRES        #0
  .define         HIRES         #1
  .define         PAGE_1        #1
  .define         PAGE_2        #2

.BSS
Page_Current:       .byte       0
Page_Future:        .byte       0

.DATA
;Pages are composed of lines. Here are their 1st pixel address
Page1:  .word $400, $480, $500, $580, $600, $680, $700, $780, $428, $4A8, $528, $5A8, $628, $6A8, $728, $7A8, $450, $4D0, $550, $5D0, $650, $6D0, $750, $7D0
Page2:  .word $800, $880, $900, $980, $A00, $A80, $B00, $B80, $828, $8A8, $928, $9A8, $A28, $AA8, $B28, $BA8, $850, $8D0, $950, $9D0, $A50, $AD0, $B50, $BD0


.CODE


;will put the adress of the future page into ptr1
.macro GET_PAGE_FUTURE
  .local get_page_future_page_1, get_page_future_page_2, get_page_future_end
  LDA Page_Current
  CMP PAGE_1
  BEQ get_page_future_page_2
  ;storing the low and high bytes of the page address into ptr1
  get_page_future_page_1:
      LDA #<Page1
      STA ptr1
      LDA #>Page1
      STA ptr1+1
      CLC
      BCC get_page_future_end
  get_page_future_page_2:
      LDA #<Page2
      STA ptr1
      LDA #>Page1
      STA ptr1+1
  get_page_future_end:
.endmacro




;**************************
;void __fastcall__ mode_text( void );
_mode_text:
  LDA $C054 ;page 1
  LDA $C051 ;text
  RTS




;**************************
;void __fastcall__ gfx_init( uint8_t RES, uint8_t MODE )
; Switches to the desired res and mode.
; Always displays page 1
_gfx_init:
res:    TAX
        LDY #0
        LDA (sp),Y
        CMP LOWRES
        BNE hires
lores:  LDA $C056
        CLC
        BCC mode
hires:  LDA $C057
mode:   CPX FULLSCREEN
        BNE split
fullscr:LDA $C052
        CLC
        BCC init_end
split:  LDA $C053
init_end:
        LDA $C054 ;page 1
        LDA $C050 ;graphics
        LDA PAGE_1
        STA Page_Current
        JSR popa  ;the parameter on stack
        RTS





;**************************
;void __fastcall__ gfx_fill( uint8_t color )
; Fills the screen with the given color
_gfx_fill:
        ; TODO SWITCH PAGES !!
        ;saving the context
        ;WARNING context us not saved properly: ptr+1 not saved!
        TAX
        LDA tmp1
        JSR pusha
        LDA ptr1
        JSR pusha
        LDA ptr2
        JSR pusha
        TXA
        ;duplicate nybble color
        STA tmp1
        ASL A
        ASL A
        ASL A
        ASL A
        ORA tmp1
        STA tmp1
        ;select to page to write into
        LDA Page_Current
        CMP PAGE_1
        BNE page_2
        ;storing the low and high bytes of the page address into ptr1
page_1: LDA #<Page1
        STA ptr1
        LDA #>Page1
        STA ptr1+1
        CLC
        BCC fill
page_2: LDA #<Page2
        STA ptr1
        LDA #>Page1
        STA ptr1+1
fill:
        ;iterating on X: the line number
for_lines:
        ;getting the 16 bit line adress and storing it in ptr2
        TXA
        ASL
        TAY
        LDA (ptr1),Y
        STA ptr2
        INY
        LDA (ptr1),Y
        STA ptr2+1
        ;iterating on Y to fill a line
        LDY #0
fill_line:
        LDA tmp1           ;reload the color
        STA (ptr2),Y       ;color the current pixel
        INY                ;next pixel
        CPY #40            ;nb pixels per line
        BNE fill_line
        INX
        CPX #20            ;nb lines
        BNE for_lines
fill_end:
        ;displaying the page we filled
        ; TODO SWITCH PAGES !!
        ;restoring the context
        JSR popa
        STA ptr2
        JSR popa
        STA ptr1
        JSR popa
        STA tmp1
        RTS


;**************************
;void __fastcall__ gfx_pixel( uint8_t color, uint8_t coord_x, uint8_t coord_y )
; Draws a pixel at the given coordinates on the Page_Future
_gfx_pixel:
        ;aliases
        coord_y := tmp3
        ;saving the context
        PHA
        LDA tmp3
        JSR pusha
        LDA tmp2
        JSR pusha
        LDA tmp1
        JSR pusha
        LDA ptr1
        LDX ptr1+1
        JSR pushax
        LDA ptr2
        LDX ptr2+1
        JSR pushax
        PLA

        STA coord_y    ;coord_y
        ; Modify the color depending on the pixel's line parity
        AND #1
        BEQ even
odd:      ; pixel is on an odd line
          LDA #$0F    ;To or with the other nybble when writting the pixel
          STA tmp1
          LDY #(1+7)  ;there were 7 pushes to save registers
          LDA (sp),Y  ;color
          ASL         ;shift the color to put it on high nybble
          ASL
          ASL
          ASL
          CLC
          BCC oddeven
even:     ; pixel is on an even line
          LDA #$F0    ;To or with the other nybble when writting the pixel
          STA tmp1
          LDY #(1+7)  ;there were 7 pushes to save registers
          LDA (sp),Y  ;color
oddeven:
        STA tmp2

        ; FIXME : switching to Page 2 does not work
        ; dont get the future page and work on page 1
        ;GET_PAGE_FUTURE
        LDA #<Page1
        STA ptr1
        LDA #>Page1
        STA ptr1+1

        ; get the line adress and store it in ptr2
        LDA coord_y
        LSR
        ASL
        TAY
        LDA (ptr1),Y
        STA ptr2
        INY
        LDA (ptr1),Y
        STA ptr2+1

        ; get coord_x
        LDY #7      ;there were 4 pushes to save registers
        LDA (sp),Y
        ; draw
        TAY
        LDA (ptr2),Y  ;a color may have been present on the other nybble
        AND tmp1      ;clearing the pixel's nybble
        ORA tmp2      ;adding the color to the pixel's nyyble
        STA (ptr2),Y

        ;restoring the context
        JSR popax
        STA ptr2
        STX ptr2+1
        JSR popax
        STA ptr1
        STX ptr1+1
        JSR popa
        STA tmp1
        JSR popa
        STA tmp2
        JSR popa
        STA tmp3
        JSR popa  ;the 2 parameters on stack
        JSR popa
        RTS


;**************************
;uint8_t __fastcall__ gfx_get_pixel( uint8_t coord_x, uint8_t coord_y )
; Returns the pixel's color
_gfx_get_pixel:
        TAX         ; coord_y

        ;saving the context
        LDA ptr1
        JSR pusha
        LDA ptr2
        JSR pusha

        ; Page's address to ptr1
        LDA #<Page1
        STA ptr1
        LDA #>Page1
        STA ptr1+1
        ; Line's adress to ptr2
        TXA
        LSR
        ASL
        TAY
        LDA (ptr1),Y
        STA ptr2
        INY
        LDA (ptr1),Y
        STA ptr2+1
        ; get coord_x
        LDY #2      ;there were 4 pushes to save registers
        LDA (sp),Y
        TAY
        TXA
        AND #1  ; test line parity to read the correct nybble
        BEQ even_2
odd_2:    LDA (ptr2),Y
          LSR  ; color is in lo nybble
          LSR
          LSR
          LSR
          CLC
          BCC end_2
even_2:   LDA (ptr2),Y
          AND #$0F  ; color is in hi nybble
end_2:  TAX

        ;restoring the context
        JSR popa
        STA ptr2
        JSR popa
        STA ptr1
        JSR popa  ;1st parameter

        ; return value
        TXA
        LDX #0

        RTS


;**************************
;void __fastcall__ gfx_refresh( void )
; Updates the screen by displaying the Future Page which becomes Current
_gfx_refresh:
        LDA Page_Current
        CMP PAGE_1
        BEQ switch_to_page_2

switch_to_page_1:
          LDA $C054 ;page 1
          LDA PAGE_1
          CLC
          BCC switch_to_page_end
switch_to_page_2:
          LDA $C055 ;page 2
          LDA PAGE_2

switch_to_page_end:
        STA Page_Current
        RTS
