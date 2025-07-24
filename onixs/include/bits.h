#ifndef BITS_H_
#define BITS_H_
#define BV(x) (1 << (x))
#define BIT_SET(reg, x) (reg) |= (1 << (x))
#define BIT_RESET(reg, x) (reg) &= ~(1 << (x))
#define BIT_READ(reg, x) ((reg) & (1 << (x)))
#endif
