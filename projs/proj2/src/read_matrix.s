.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
#   - If malloc returns an error,
#     this function terminates the program with error code 26
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fread error or eof,
#     this function terminates the program with error code 29
# ==============================================================================
read_matrix:
  addi sp, sp, -20
  sw ra, 0(sp)
  sw s1, 4(sp)
  sw s2, 8(sp)
  sw s3, 12(sp)
  sw s4, 16(sp)

  mv s1, a1
  mv s2, a2

  li a1, 0
  call fopen
  blt a0, x0, Efopen
  mv s3, a0

  addi sp, sp, -8
  mv s4, sp

  mv a0, s3
  mv a1, s4
  li a2, 4
  call fread
  li t1, 4
  bne t1, a0, Efread

  addi s4, s4, 4

  mv a0, s3
  mv a1, s4
  li a2, 4
  call fread
  li t1, 4
  bne t1, a0, Efread

  lw t0, 0(sp)
  lw t1, 4(sp)
  addi sp, sp, 8
  sw t0, 0(s1)
  sw t1, 0(s2)
  mv s1, t0
  mv s2, t1

  mul a0, s1, s2
  slli a0, a0, 2
  call malloc
  beq a0, x0, Emalloc
  mv s4, a0

  li t0, 0
  li t1, 0
loop_start:
  mul t2, t0, s2
  add t2, t2, t1
  slli t2, t2, 2
  add t2, t2, s4

  mv a0, s3
  mv a1, t2
  li a2, 4

  addi sp, sp, -8
  sw t0, 0(sp)
  sw t1, 4(sp)

  call fread

  lw t1, 4(sp)
  lw t0, 0(sp)
  addi sp, sp, 8

  li t4, 4
  bne t4, a0, Efread

  addi t1, t1, 1
  bne t1, s2, loop_start
  addi t0, t0, 1
  li t1, 0
  bne t0, s1, loop_start

  mv a0, s3
  call fclose
  bne x0, a0, Efclose

  mv a0, s4

  lw s4, 16(sp)
  lw s3, 12(sp)
  lw s2, 8(sp)
  lw s1, 4(sp)
  lw ra, 0(sp)
  addi sp, sp, 20

  ret
Emalloc:
  li a0, 26
  j exit
Efopen:
  li a0, 27
  j exit
Efclose:
  li a0, 28
  j exit
Efread:
  li a0, 29
  j exit
