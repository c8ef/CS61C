.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int arrays
# Arguments:
#   a0 (int*) is the pointer to the start of arr0
#   a1 (int*) is the pointer to the start of arr1
#   a2 (int)  is the number of elements to use
#   a3 (int)  is the stride of arr0
#   a4 (int)  is the stride of arr1
# Returns:
#   a0 (int)  is the dot product of arr0 and arr1
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
#   - If the stride of either array is less than 1,
#     this function terminates the program with error code 37
# =======================================================
dot:
  ble a2, x0, E1
  ble a3, x0, E2
  ble a4, x0, E2

  addi sp, sp, -8
  sw s0, 0(sp)
  sw s1, 4(sp)

  mv s0, a0
  mv s1, a1

  slli a3, a3, 2
  slli a4, a4, 2
  li a0, 0
  li t1, 0
loop_start:
  mul t0, a3, t1
  mul t2, a4, t1
  add t3, s0, t0
  add t4, s1, t2
  lw t3, 0(t3)
  lw t4, 0(t4)
  mul t5, t3, t4
  add a0, a0, t5
  
  addi t1, t1, 1
  bne t1, a2, loop_start

  lw s1, 4(sp)
  lw s0, 0(sp)
  addi sp, sp, 8
	ret
E1:
  li a0, 36
  j exit
E2:
  li a0, 37
  j exit