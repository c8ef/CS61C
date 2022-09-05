.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
#   a0 (int*) is the pointer to the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   None
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# ==============================================================================
relu:
  ble a1, x0, E
  li t0, 0
loop_start:
  slli t1, t0, 2
  add t2, t1, a0
  lw t3, 0(t2)
  bge t3, x0, loop_continue
  sw x0, 0(t2)

loop_continue:
  addi t0, t0, 1
  bne t0, a1, loop_start
  ret
E:
  li a0, 36
  j exit