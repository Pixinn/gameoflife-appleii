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


  .include        "apple2.inc"
  .include        "zeropage.inc"

  .import         pusha
  .import         popa

  .export _init_rnd_color
  .export _get_color


.BSS
Colors: .dword  $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0
         .dword  $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0
         .dword  $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0
         .dword  $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0, $0

Color:   .byte $0

.CODE

; void __fastcall__ init_rnd_color( void )
; Fills Colors with 8bits random values
; Code inspired by: http://www.codebase64.org/doku.php?id=base:small_fast_8-bit_prng
_init_rnd_color:
seed := tmp1
    LDA tmp1
    JSR pusha

    LDA #123
    STA seed
    LDX #0
loop: ; for X = 0 to 255
      LDA seed
        BEQ doEor
      ASL
      BEQ noEor ;if the input was $80, skip the EOR
      BCC noEor
doEor:
      EOR #$2B
noEor:
      STA seed
      AND #$0F ; color is in lo-nybble
        BEQ loop ; 0 (BLACK) not accepted
      CMP #$0F
        BEQ loop ; 0xF (WHITE) not accepted
      STA Colors,X
      INX     ; next X
      CPX #0
      BEQ loop_end
      CLC
      BCC loop
loop_end:

      JSR popa
      STA tmp1
      RTS



; uint8_t __fastcall__  get_color( void )
; Gets the next color from the buffer
_get_color:
        LDX Color
        LDA Colors,X
        INX
        STX Color
        LDX #0
        RTS
