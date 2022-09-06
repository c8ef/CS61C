.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fwrite error or eof,
#     this function terminates the program with error code 30
# ==============================================================================
write_matrix:
  addi sp, sp, -24
  sw ra, 0(sp)
  sw s0, 4(sp)
  sw s1, 8(sp)
  sw s2, 12(sp)
  sw s3, 16(sp)
  sw s4, 20(sp)

  mv s2, a2
  mv s3, a3
  mv s1, a1

  li a1, 1
  call fopen
  blt a0, x0, Efopen
  mv s0, a0

  addi sp, sp, -8
  sw s2, 0(sp)
  sw s3, 4(sp)
  mv s4, sp

  mv a0, s0
  mv a1, s4
  li a2, 2
  li a3, 4
  call fwrite
  li t1, 2
  bne t1, a0, Efwrite
  addi sp, sp, 8

  mul t1, s2, s3
  mv a0, s0
  mv a1, s1
  mv a2, t1
  li a3, 4

  addi sp, sp, -4
  sw t1, 0(sp)

  call fwrite

  lw t1, 0(sp)
  addi sp, sp, 4

  bne t1, a0, Efwrite

  mv a0, s0
  call fclose
  bne a0, x0, Efclose

  lw s4, 20(sp)
  lw s3, 16(sp)
  lw s2, 12(sp)
  lw s1, 8(sp)
  lw s0, 4(sp)
  lw ra, 0(sp)
  addi sp, sp, 24

  ret
Efopen:
  li a0, 27
  j exit
Efclose:
  li a0, 28
  j exit
Efwrite:
  li a0, 30
  j exit
