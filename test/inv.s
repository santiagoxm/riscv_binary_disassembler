inv: 
addi sp, sp, -4
sw ra, (0)sp

xori, a0, a0, -1
addi, a0, a0, 1

lw ra, (0)sp
addi sp, sp, 4
ret