  global  update

  section .text

; *************************************************************
; *   void update(double *max, int *max_i, double m, int i)   *
; *************************************************************
update:
  ;rdi  <- *max
  ;rsi  <- *max_i
  ;xmm0 <- m
  ;edx  <- i
  push    rbp
  mov     rbp, rsp

  mov     rax, rdi                  ;move max pointer
  movsd   QWORD [rax], xmm0         ;move m to address of max
  
  mov     rax, rsi                  ;move max_i pointer
  mov     DWORD [rax], edx          ;move i to address of max_i
  
  nop
  pop     rbp
  ret