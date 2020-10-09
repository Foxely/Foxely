.global _main

  .text

plus:
  ADD RDI, RSI
  MOVq $RAX, $RDI
  RET

_main:
  PUSH RDI
  PUSH RSI
  MOVl $3, RDI
  PUSH RDI
  PUSH RSI
  MOVl $2, RDI
  MOVl $1, RSI
  CALL plus
  POP RSI
  POP RDI
  MOV RSI, RAX

  CALL plus
  POP RSI
  POP RDI

  MOV RDI, RAX
  MOV RAX, 0x2000001
  SYSCALL