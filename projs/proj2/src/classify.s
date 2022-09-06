.globl classify

.text
# =====================================
# COMMAND LINE ARGUMENTS
# =====================================
# Args:
#   a0 (int)        argc
#   a1 (char**)     argv
#   a1[1] (char*)   pointer to the filepath string of m0
#   a1[2] (char*)   pointer to the filepath string of m1
#   a1[3] (char*)   pointer to the filepath string of input matrix
#   a1[4] (char*)   pointer to the filepath string of output file
#   a2 (int)        silent mode, if this is 1, you should not print
#                   anything. Otherwise, you should print the
#                   classification and a newline.
# Returns:
#   a0 (int)        Classification
# Exceptions:
#   - If there are an incorrect number of command line args,
#     this function terminates the program with exit code 31
#   - If malloc fails, this function terminates the program with exit code 26
#
# Usage:
#   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>
classify:
  li t1, 5
  bne t1, a0, Eargc

  addi sp, sp, -40
  sw ra, 0(sp)
  sw s0, 4(sp)
  sw s1, 8(sp)
  sw s2, 12(sp)
  sw s3, 16(sp)
  sw s4, 20(sp)
  sw s5, 24(sp)
  sw s6, 28(sp)
  sw s7, 32(sp)
  sw s8, 36(sp)

  mv s1, a1
  mv s2, a2

  li a0, 24
  call malloc
  beq a0, x0, Emalloc
  mv s3, a0

  # Read pretrained m0
  lw a0, 4(s1)
  mv t1, s3
  addi t2, s3, 4
  mv a1, t1
  mv a2, t2
  call read_matrix
  mv s0, a0

  # Read pretrained m1
  lw a0, 8(s1)
  addi t1, s3, 8
  addi t2, s3, 12
  mv a1, t1
  mv a2, t2
  call read_matrix
  mv s4, a0

  # Read input matrix
  lw a0, 12(s1)
  addi t1, s3, 16
  addi t2, s3, 20
  mv a1, t1
  mv a2, t2
  call read_matrix
  mv s5, a0

  # Compute h = matmul(m0, input)
  lw t1, 0(s3)
  lw t2, 20(s3)
  mul a0, t1, t2
  slli a0, a0, 2
  call malloc
  beq a0, x0, Emalloc
  mv s6, a0

  mv a0, s0
  lw a1, 0(s3)
  lw a2, 4(s3)
  mv a3, s5
  lw a4, 16(s3)
  lw a5, 20(s3)
  mv a6, s6
  call matmul

  # Compute h = relu(h)
  mv a0, s6
  lw t1, 0(s3)
  lw t2, 20(s3)
  mul a1, t1, t2
  call relu
  # Compute o = matmul(m1, h)
  lw t1, 8(s3)
  lw t2, 20(s3)
  mul a0, t1, t2
  slli a0, a0, 2
  call malloc
  beq a0, x0, Emalloc
  mv s7, a0

  mv a0, s4
  lw a1, 8(s3)
  lw a2, 12(s3)
  mv a3, s6
  lw a4, 0(s3)
  lw a5, 20(s3)
  mv a6, s7
  call matmul

  # Write output matrix o
  lw a0, 16(s1)
  mv a1, s7
  lw a2, 8(s3)
  lw a3, 20(s3)
  call write_matrix

  # Compute and return argmax(o)
  lw t1, 8(s3)
  lw t2, 20(s3)
  mul a1, t1, t2
  mv a0, s7
  call argmax
  mv s8, a0

  # If enabled, print argmax(o) and newline
  bne s2, x0, freememory
  mv a0, s8
  call print_int
  li a0, 10
  call print_char
freememory:
  mv a0, s3
  call free
  mv a0, s6
  call free
  mv a0, s7
  call free

  mv a0, s8

  lw ra, 0(sp)
  lw s0, 4(sp)
  lw s1, 8(sp)
  lw s2, 12(sp)
  lw s3, 16(sp)
  lw s4, 20(sp)
  lw s5, 24(sp)
  lw s6, 28(sp)
  lw s7, 32(sp)
  lw s8, 36(sp)
  addi sp, sp, 40
  ret
Emalloc:
  li a0, 26
  j exit
Eargc:
  li a0, 31
  j exit
