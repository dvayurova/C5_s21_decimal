#include "s21_decimal.h"

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  memset(dst, 0, sizeof(*dst));
  int resCode = 0, f_scale = 0;
  if (src != NAN && src != INFINITY && src != 0) {
    if (src < 0) {
      src *= -1;
      setSign(dst, 1);
    } else
      setSign(dst, 0);
    int binExp = float_getScale(src);
    if (binExp > 95)
      resCode = 1;
    if (src < 1e8 && !resCode) {
      for (; !((int)src); src *= 10, f_scale++) {
      }
      for (; src < 1e7;) {
        src *= 10;
        f_scale++;
      }
      binExp = float_getScale(src);
    }
    if (f_scale > 28 || binExp < -94)
      resCode = 1;
    if (!resCode) {
      setBit(dst, binExp, 1);
      unsigned int fbits = *((unsigned int *)&src);
      for (unsigned int mask = 0x400000; mask; mask >>= 1) {
        binExp--;
        if (!!(fbits & mask))
          setBit(dst, binExp, 1);
      }
      setScale(dst, f_scale);
    }
  } else
    resCode = (src == 0) ? 0 : 1;
  return resCode;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int resCode = 0;
  double res = 0;
  *dst = 0;
  for (int i = 0; i < 96; i++) {
    if (getBit(src, i))
      res += pow(2, i);
  }
  res *= pow(10, (-1 * (getScale(src))));
  *dst = res;
  if (getSign(src))
    *dst *= -1;
  return resCode;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  memset(dst, 0, sizeof(*dst));
  if (src < 0) {
    setSign(dst, 1);
    src *= -1;
  }
  dst->bits[LOW] = src;
  return 0;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int resCode = 0;

  // printf("\n ===src===: \n");
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBit(src, i));
  // }
  // printf("\n src.bits[3]:\n");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBit(src, i));
  // }

  if (first_bit(src) > 31)
    resCode = 1;
  else {
    *dst = 0;
    for (int i = 0; i < 96; i++) {
      if (getBit(src, i))
        *dst += pow(2, i);
    }
    *dst *= pow(10, (getScale(src) * -1));
    if (getSign(src))
      *dst *= -1;
  }

  // printf("\n dst = %d", *dst);

  return resCode;
}
