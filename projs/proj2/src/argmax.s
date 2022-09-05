.globl argmax

.text
# =================================================================
# FUNCTION: Given a int array, return the index of the largest
#   element. If there are multiple, return the one
#   with the smallest index.
# Arguments:
#   a0 (int*) is the pointer to the start of the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   a0 (int)  is the first index of the largest element
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# =================================================================
argmax:
  ble a1, x0, E

  addi sp, sp, -8
  sw s0, 0(sp)
  sw s1, 4(sp)

  lw s0, 0(a0)
  mv s1, a0
  li a0, 0
  li t0, 1
  j loop_continue
loop_start:
  slli t1, t0, 2
  add t2, t1, s1
  lw t3, 0(t2)
  blt t3, s0, adder
  mv s0, t3
  mv a0, t0
adder:
  addi t0, t0, 1
loop_continue:
  bne t0, a1, loop_start

  lw s1, 4(sp)
  lw s0, 0(sp)
  addi sp, sp, 8

  ret
E:
  li a0, 36
  j exit