#include "bit_ops.h"
#include <stdint.h>
#include <stdio.h>

/*
  Returns the n-th bit of x.
  Assumes 0 <= n <= 31.
*/
uint32_t get_bit(uint32_t x, uint32_t n) { return (x >> n) & 1; }

/*
  Set the n-th bit of *x to v.
  Assumes 0 <= n <= 31, and v is 0 or 1.
*/
void set_bit(uint32_t *x, uint32_t n, uint32_t v) {
  (*x) &= ~(1 << n);
  (*x) |= (v << n);
}

/*
  Flips the n-th bit of *x.
  Assumes 0 <= n <= 31.
*/
void flip_bit(uint32_t *x, uint32_t n) { set_bit(x, n, !get_bit(*x, n)); }
