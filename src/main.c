#include <limits.h>
#include <stdio.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

int getBits(s21_decimal d, int i) { return !!(d.bits[i / 32] & (1 << i % 32)); }

void setBits(s21_decimal *d, int i, int bit) {
  if (bit == 1)
    d->bits[i / 32] = d->bits[i / 32] | (1 << i % 32);
  else if (bit == 0)
    d->bits[i / 32] = d->bits[i / 32] & ~(1 << i % 32);
}

int getSign(s21_decimal d) {
  int sign = 1;
  if ((d.bits[3] & (1 << 31)) == 0)
    sign = 0;
  return sign;
}

void setSign(s21_decimal *d, int sign) {
  if (sign == 1)
    d->bits[3] = d->bits[3] | (1 << 31);
  else if (sign == 0)
    d->bits[3] = d->bits[3] & ~(1 << 31);
}

void binary_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  int tmp = 0;
  for (int i = 0; i < 96; i++) {
    if (getBits(value_1, i) && getBits(value_2, i) && tmp)
      setBits(res, i, 1);
    else if (getBits(value_1, i) && getBits(value_2, i) && !tmp)
      tmp = 1;
    else if ((getBits(value_1, i) + getBits(value_2, i)) == 1 && !tmp)
      setBits(res, i, 1);
    else if (!getBits(value_1, i) && !getBits(value_2, i) && tmp) {
      setBits(res, i, 1);
      tmp = 0;
    }
  }
}

int main() {
  s21_decimal dec;
  dec.bits[0] = 5;
  dec.bits[1] = 0;
  dec.bits[2] = 0;
  dec.bits[3] = 0b00000000000000010000000000000000;
  // printf("\nSign=%d\n", getSign(dec));
  printf("num=%d, bits:\n", dec.bits[0]);
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBits(dec, i));
  }
  // setBits(&dec, 0, 0);
  // printf("\nnum after=%d, bits:\n", dec.bits[0]);
  // for (int i = 95; i >= 0; i--) {
  //   printf("%d", getBits(dec, i));
  // }
  // setSign(&dec, 1);
  // printf("\nbits[3]=");
  // for (int i = 127; i >= 96; i--) {
  //   printf("%d", getBits(dec, i));
  // }
  // printf("\nSign=%d\n", getSign(dec));
  s21_decimal val2;
  val2.bits[0] = 2;
  val2.bits[1] = 0;
  val2.bits[2] = 0;
  val2.bits[3] = 0b00000000000000010000000000000000;
  printf("\nnum=%d, bits:\n", val2.bits[0]);
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBits(val2, i));
  }
  s21_decimal res = {0};
  binary_sum(dec, val2, &res);
  printf("\nnum=%d, bits:\n", res.bits[0]);
  for (int i = 95; i >= 0; i--) {
    printf("%d", getBits(res, i));
  }
}
