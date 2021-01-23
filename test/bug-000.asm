
stk 6
org 0

db 0
db 1

&name
db 0

#call("write_to_name")

mov r1, *name
dec r1

@printl
    rcl r2, r1
    jz  r2, %printle
    out r2
    inc r1
    jmp %printl
@printle

end

@write_to_name
; takes 0 arguments, returns 0 arguments
; assumes input terminated with periodtt
; writes directly to memory

  psh r1
  psh r2
  
  mov r1, *name  
  @in_f_loop
    in  r2
    ceq r2, ..
    cjn %quit_in_f_loop
    sto r1, r2
    inc r1
    jmp %in_f_loop
  @quit_in_f_loop
  
  inc r1
  db 0
  
  pop r2
  pop r1
  ret

