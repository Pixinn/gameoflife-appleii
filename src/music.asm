
.include        "apple2.inc"
.include        "zeropage.inc"


.import         popa
.export         _note
.export         _pause

.DATA
Loops:  .byte 149, 141, 133, 125, 118, 111, 105, 99, 94, 88, 83, 78 ;Note loops
Test:   .word   31100

;NB nop loops of each note, lasting hal a period
Half_Periods:     .byte  251, 237, 224, 211, 199, 188, 177, 167, 158
                  .byte  149, 140, 132, 125, 118, 111, 105, 99, 93, 88, 83, 78
                  .byte  74, 69, 65, 62, 58, 55, 52, 49, 46, 43, 41, 38
;NB duration loop for each note to last for 1 second
Durations:        .word  311, 330, 349, 370, 392, 415, 440, 466, 494
                  .word  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988
                  .word  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976
;Inner and Outer loop of silence to last for 1 second
Silence_Inner:    .byte  125
Silence_Outer:    .word  622

.ALIGN 256
.CODE

  ; ******************
  ; \param idx_note : index of the note to play, used to fetch values in Half_Periods and Durations tables
  ;void __fastcall__ note( uint8_t nb_shift_duration, uint8_t idx_note )
_note:
  ;aliases
  half_period := tmp1
  duration_lo := tmp2
  duration_hi := tmp3

  ;processing parameters
  TAX       ; idx_note in X
  LDA Half_Periods, X
  STA half_period
  TXA
  ASL ;Durations are on 16 bits
  TAX
  LDA Durations, X
  STA duration_lo
  LDA Durations+1, X
  STA duration_hi
  JSR popa  ; nb_shift_duration in A
  TAY
  BEQ end_shift
loop_shift: ;shifting the duration
  LSR duration_hi
  ROR duration_lo
  DEY
  BNE loop_shift
end_shift:

  ;Init loop duration
  CLC
  LDY duration_lo
  INC duration_hi
  ;Starting to vibe ;)
  ;These loop should fit in one page to respect the timings
loop_half_period:
  LDA $C030             ;4 cycles //SPEAKER
  LDX half_period       ;3 cycles
loop_nops:
  NOP                   ;2 cycles
  NOP                   ;2 cycles
  NOP                   ;2 cycles
  NOP                   ;2 cycles
  DEX                   ;2 cycles
  BNE loop_nops         ;3 cycles
  ;Testing duration loop
  DEY                   ;2 cycles
  BNE loop_half_period  ;3 cycles
  DEC duration_hi       ;5 cycles
  BNE loop_half_period  ;3 cycles

end:
  RTS

.ALIGN 256

  ; ******************
  ;void __fastcall__ pause( uint8_t nb_shift_duration )
_pause:

;DEBUG:
;  CLC
;  BCC DEBUG
;  ; ++++++ DEBUG

  ;aliases
  pause_lo := tmp2
  pause_hi := tmp3

  TAY       ; duration shift in Y
  INY
  LDA Silence_Outer
  STA pause_lo
  LDA Silence_Outer+1
  STA pause_hi
  DEY
  BEQ end_shift_silence
loop_shift_silence: ;shifting the duration
  LSR pause_hi
  ROR pause_lo
  DEY
  BNE loop_shift_silence
end_shift_silence:

  ;Init loop duration
  CLC
  LDY pause_lo
  INC pause_hi
  ;Starting to be mute
  ;These loop should fit in one page to respect the timings
loop_silence:
  LDX Silence_Inner     ;4 cycles
loop_nops_silence:
  NOP                   ;2 cycles
  NOP                   ;2 cycles
  NOP                   ;2 cycles
  NOP                   ;2 cycles
  DEX                   ;2 cycles
  BNE loop_nops_silence ;3 cycles
  ;Testing duration loop
  DEY                   ;2 cycles
  BNE loop_silence      ;3 cycles
  DEC pause_hi          ;5 cycles
  BNE loop_silence      ;3 cycles

  RTS
