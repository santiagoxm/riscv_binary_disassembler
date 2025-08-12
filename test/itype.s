addi zero, a1, 1
xori ra, a1, 2
ori a0, a1, 3
andi a0, a1, -513
slli sp, a1, 23
srli a0, a1, 4
srai a0, a1, 0
slti tp, a1, 2
sltiu a0, a1, 745

lb a2, 5(zero)
lh t0, 3(a1)
lw t1, 0(zero)
lbu t6, -4(zero)
lhu a1, 243(t0)

jalr a1, a2, 4

ecall
