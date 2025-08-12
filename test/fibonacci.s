fib:
addi sp, sp, -4
sw ra, 0(sp)
li t0, 0
li t1, 1
loop:
beqz a0, return 
add t2, t0, t1
mv t0, t1
mv t1, t2
addi a0, a0, -1
j loop

return: 
mv a0, t0 
lw ra, 0(sp)
addi sp, sp, 4
ret
