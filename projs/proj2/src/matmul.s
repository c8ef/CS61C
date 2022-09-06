.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
#   d = matmul(m0, m1)
# Arguments:
#   a0 (int*)  is the pointer to the start of m0
#   a1 (int)   is the # of rows (height) of m0
#   a2 (int)   is the # of columns (width) of m0
#   a3 (int*)  is the pointer to the start of m1
#   a4 (int)   is the # of rows (height) of m1
#   a5 (int)   is the # of columns (width) of m1
#   a6 (int*)  is the pointer to the the start of d
# Returns:
#   None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 38
# =======================================================
matmul:
  ble a1, x0, E
  ble a2, x0, E
  ble a4, x0, E
  ble a5, x0, E
  bne a2, a4, E

  addi sp, sp, -28
  sw s0, 0(sp)
  sw s1, 4(sp)
  sw s2, 8(sp)
  sw s3, 12(sp)
  sw s5, 16(sp)
  sw s6, 20(sp)
  # when calling another function, always store the return address
  sw ra, 24(sp)

  mv s0, a0
  mv s1, a1
  mv s2, a2
  mv s3, a3
  mv s5, a5
  mv s6, a6
  li t0, 0
  li t1, 0
loop_start:
  mul t2, s2, t0
  slli t2, t2, 2
  add t2, t2, s0
  
  slli t3, t1, 2
  add t3, t3, s3

  mv a0, t2
  mv a1, t3
  mv a2, s2
  li a3, 1
  mv a4, s5
  
  addi sp, sp, -8
  sw t0, 0(sp)
  sw t1, 4(sp)
  call dot
  lw t1, 4(sp)
  lw t0, 0(sp)
  addi sp, sp, 8

  sw a0, 0(s6)
  addi s6, s6, 4

  addi t1, t1, 1
  bne t1, s5, loop_start
  addi t0, t0, 1
  li t1, 0
  bne t0, s1, loop_start

  lw ra, 24(sp)
  lw s6, 20(sp)
  lw s5, 16(sp)
  lw s3, 12(sp)
  lw s2, 8(sp)
  lw s1, 4(sp)
  lw s0, 0(sp)
  addi sp, sp, 28

  ret
E:
  li a0, 38
  j exit