#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  unsigned int bits[4];
} s21_decimal;

int getBit(s21_decimal d, int i) { return !!(d.bits[i / 32] & (1 << i % 32)); }

void setBits(s21_decimal *d, int i, int bit) {
  if (bit == 1)
    d->bits[i / 32] = d->bits[i / 32] | (1 << i % 32);
  else if (bit == 0)
    d->bits[i / 32] = d->bits[i / 32] & ~(1 << i % 32);
}

int getSign(s21_decimal d) {
  int sign = 1;
  if ((d.bits[3] & (1 << 31)) == 0) sign = 0;
  return sign;
}

void setSign(s21_decimal *d, int sign) {
  if (sign == 1)
    d->bits[3] = d->bits[3] | (1 << 31);
  else if (sign == 0)
    d->bits[3] = d->bits[3] & ~(1 << 31);
}

int getScale(s21_decimal x) {
  int bit = 0;
  int rez = 0;
  for (int i = 112; i < 117; i++) {
    bit = getBit(x, i);
    if (bit) rez += pow(2, (i - 112));
  }
  return rez;
}

int setScale(s21_decimal *d, int scale) {
  int bit = 0;
  if (scale <= 28 && scale >= 0) {
    for (int i = 4; i >= 0; i--) {
      bit = !!(scale & (1 << i));
      setBits(d, 112 + i, bit);
    }
    bit = 1;
  }
  return bit;  // if scale < 0 || > 28 return 0
}

// binary_sum - алгоритм суммирования положительных целых чисел в двоичной
// системе
int binary_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  memset(res, 0, sizeof(*res));
  int tmp = 0;
  for (int i = 0; i < 96; i++) {
    if (getBit(value_1, i) && getBit(value_2, i) && tmp)
      setBits(res, i, 1);
    else if (getBit(value_1, i) && getBit(value_2, i) && !tmp)
      tmp = 1;
    else if ((getBit(value_1, i) + getBit(value_2, i)) == 1 && !tmp)
      setBits(res, i, 1);
    else if (!getBit(value_1, i) && !getBit(value_2, i) && tmp) {
      setBits(res, i, 1);
      tmp = 0;
    }
  }
  return tmp;  // возвращает tmp для отслеживания переполнения
}

// dop_Code - перевод в дополнительный код (для вычитания)
void dop_Code(s21_decimal value, s21_decimal *dop_code, int first_index) {
  for (int i = first_index; i >= 0; i--) {
    setBits(dop_code, i, getBit(value, i) ? 0 : 1);
  }
  dop_code->bits[0] += 1;
}

int first_bit(s21_decimal dec) {  // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 95; i >= 0 && !first; i--) {
    if (getBit(dec, i)) first = i;
  }
  return first;
}

// binary_sub - алгоритм вычитания положительных целых чисел в двоичной
// системе
void binary_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *res) {
  s21_decimal val2_dop_code = {0};
  int first_Bit = first_bit(value_1);
  dop_Code(value_2, &val2_dop_code, first_Bit);
  binary_sum(value_1, val2_dop_code, res);
  if (getBit(*res, first_Bit + 1)) setBits(res, first_Bit + 1, 0);
}

void copy_decimal(s21_decimal src, s21_decimal *dst) {
  dst->bits[0] = src.bits[0];
  dst->bits[1] = src.bits[1];
  dst->bits[2] = src.bits[3];
  dst->bits[3] = src.bits[3];
}

int scale_up(s21_decimal *value_smallScale, int bigger_scale,
             int smaller_scale) {
  int is_inf = 0;
  s21_decimal tmp = {0};
  s21_decimal res = {0};
  copy_decimal(*value_smallScale, &tmp);
  while (smaller_scale < bigger_scale && !is_inf) {
    for (int i = 1; i < 10; i++) {
      is_inf = binary_sum(tmp, *value_smallScale, &res);
      copy_decimal(res, &tmp);
    }
    copy_decimal(res, value_smallScale);
    copy_decimal(*value_smallScale, &tmp);
    smaller_scale += 1;
  }
  setScale(value_smallScale, smaller_scale);
  return is_inf;  // возвращает 1 в случае переполнения
}

// ф-я выравнивания степеней - пока только повышение степени
void to_same_scale(s21_decimal *value_1, s21_decimal *value_2) {
  int scale1 = getScale(*value_1);
  int scale2 = getScale(*value_2);
  // int is_inf = 0;
  if (scale2 > scale1)
    scale_up(value_1, scale2, scale1);
  else if (scale1 > scale2)
    scale_up(value_2, scale1, scale2);
}
