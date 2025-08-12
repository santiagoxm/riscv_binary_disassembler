fact: 
addi sp, sp, -8
sw s0, 0(sp)
sw ra, 4(sp)

beqz a0, fact0
mv s0, a0

addi a0, a0, -1
jal ra, fact
mul a0, a0, s0
j ret

fact0:
li a0, 1
ret:
lw s0, 0(sp)
lw ra, 4(sp)
addi sp, sp, 8
ret
