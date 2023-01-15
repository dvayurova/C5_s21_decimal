#include "s21_decimal.h"

int getBit(s21_decimal d, int i) { return !!(d.bits[i / 32] & (1 << i % 32)); }

int setBit(s21_decimal *x, int index, int bit) {
  unsigned int mask = 1u << index % 32;
  if (bit == 1) {
    x->bits[index / 32] |= mask;
  } else {
    x->bits[index / 32] &= ~mask;
  }
  return 0;
}
int getBit_long(s21_decimal_long d, int i) {
  return !!(d.bits[i / 32] & (1 << i % 32));
}

int setBit_long(s21_decimal_long *x, int index, int bit) {
  unsigned int mask = 1u << index % 32;
  if (bit == 1) {
    x->bits[index / 32] |= mask;
  } else {
    x->bits[index / 32] &= ~mask;
  }
  return 0;
}

int getSign(s21_decimal d) {
  int sign = 1;
  if ((d.bits[3] & (1 << 31)) == 0)
    sign = 0;
  return sign;
}

int setSign(s21_decimal *x, int sign) {
  if (sign == 0)
    setBit(x, 127, 0);
  if (sign == 1)
    setBit(x, 127, 1);
  return sign;
}

int getSign_long(s21_decimal_long d) {
  int sign = 1;
  if ((d.bits[7] & (1 << 31)) == 0)
    sign = 0;
  return sign;
}

int setSign_long(s21_decimal_long *x, int sign) {
  if (sign == 0)
    setBit_long(x, 255, 0);
  if (sign == 1)
    setBit_long(x, 255, 1);
  return sign;
}
int getScale(s21_decimal x) {
  int bit = 0;
  int rez = 0;
  for (int i = 112; i < 117; i++) {
    bit = getBit(x, i);
    if (bit)
      rez += pow(2, (i - 112));
  }
  return rez;
}

int setScale(s21_decimal *d, int scale) {
  int bit = 0;
  if (scale <= 28 && scale >= 0) {
    for (int i = 4; i >= 0; i--) {
      bit = !!(scale & (1 << i));
      setBit(d, 112 + i, bit);
    }
    bit = 1;
  }
  return bit; // if scale < 0 || > 28 return 0
}

int first_bit(s21_decimal dec) { // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 95; i >= 0 && !first; i--) {
    if (getBit(dec, i))
      first = i;
  }
  return first;
}

int getScale_long(s21_decimal_long x) {
  int bit = 0;
  int rez = 0;
  for (int i = 240; i < 245; i++) {
    bit = getBit_long(x, i);
    if (bit)
      rez += pow(2, (i - 240));
  }
  return rez;
}

int setScale_long(s21_decimal_long *d, int scale) {
  int bit = 0;
  if (scale <= 28 && scale >= 0) {
    for (int i = 4; i >= 0; i--) {
      bit = !!(scale & (1 << i));
      setBit_long(d, 240 + i, bit);
    }
    bit = 1;
  }
  return bit; // if scale < 0 || > 28 return 0
}

// binary_sum - алгоритм суммирования положительных целых чисел в двоичной
// системе
int binary_sum(s21_decimal_long value_1, s21_decimal_long value_2,
               s21_decimal_long *res) {
  memset(res, 0, sizeof(*res));
  int tmp = 0;
  for (int i = 0; i < 224; i++) {
    if (getBit_long(value_1, i) && getBit_long(value_2, i) && tmp)
      setBit_long(res, i, 1);
    else if (getBit_long(value_1, i) && getBit_long(value_2, i) && !tmp)
      tmp = 1;
    else if ((getBit_long(value_1, i) + getBit_long(value_2, i)) == 1 && !tmp)
      setBit_long(res, i, 1);
    else if (!getBit_long(value_1, i) && !getBit_long(value_2, i) && tmp) {
      setBit_long(res, i, 1);
      tmp = 0;
    }
  }
  return tmp; // возвращает tmp для отслеживания переполнения
}

// dop_Code - перевод в дополнительный код (для вычитания)
void dop_Code(s21_decimal_long value, s21_decimal_long *dop_code,
              int first_index) {
  for (int i = first_index; i >= 0; i--) {
    setBit_long(dop_code, i, getBit_long(value, i) ? 0 : 1);
  }
  dop_code->bits[0] += 1;
}

int first_bit_long(
    s21_decimal_long dec) { // определяет индекс первого ненулевого бита
  int first = 0;
  for (int i = 223; i >= 0 && !first; i--) {
    if (getBit_long(dec, i))
      first = i;
  }
  return first;
}

// binary_sub - алгоритм вычитания положительных целых чисел в двоичной
// системе
void binary_sub(s21_decimal_long value_1, s21_decimal_long value_2,
                s21_decimal_long *res) {
  s21_decimal_long val2_dop_code = {0};
  memset(res, 0, sizeof(*res));
  int first_Bit = first_bit_long(value_1);
  dop_Code(value_2, &val2_dop_code, first_Bit);
  binary_sum(value_1, val2_dop_code, res);
  if (getBit_long(*res, first_Bit + 1))
    setBit_long(res, first_Bit + 1, 0);
}

void copy_decimal_long(s21_decimal_long src, s21_decimal_long *dst) {
  dst->bits[0] = src.bits[0];
  dst->bits[1] = src.bits[1];
  dst->bits[2] = src.bits[2];
  dst->bits[3] = src.bits[3];
  dst->bits[4] = src.bits[4];
  dst->bits[5] = src.bits[5];
  dst->bits[6] = src.bits[6];
  dst->bits[7] = src.bits[7];
}

void from_decimal_to_long(s21_decimal dec, s21_decimal_long *dec_long) {
  memset(dec_long, 0, sizeof(*dec_long));
  dec_long->bits[0] = dec.bits[0];
  dec_long->bits[1] = dec.bits[1];
  dec_long->bits[2] = dec.bits[2];
  dec_long->bits[3] = 0;
  dec_long->bits[4] = 0;
  dec_long->bits[5] = 0;
  dec_long->bits[6] = 0;
  dec_long->bits[7] = dec.bits[3];
}

// void from_long_to_decimal(s21_decimal *dec, s21_decimal_long dec_long) {
//   memset(dec, 0, sizeof(*dec));
//   int scale = 0;
//   scale = getScale(dec_long);
//   for (int i = 6; i != 0; i--) {
//     while (dec->bits[i] != 0) {

//     }
//   }
// }

int scale_up(s21_decimal_long *value_smallScale, int bigger_scale,
             int smaller_scale) {
  int is_inf = 0, sign = 0;
  s21_decimal_long tmp = {0};
  s21_decimal_long res = {0};
  sign = getSign_long(*value_smallScale);
  copy_decimal_long(*value_smallScale, &tmp);
  while (smaller_scale < bigger_scale && !is_inf) {
    for (int i = 1; i < 10; i++) {
      is_inf = binary_sum(tmp, *value_smallScale, &res);
      copy_decimal_long(res, &tmp);
    }
    if (!is_inf) {
      copy_decimal_long(res, value_smallScale);
      copy_decimal_long(*value_smallScale, &tmp);
      smaller_scale += 1;
    }
  }
  setScale_long(value_smallScale, smaller_scale);
  setSign_long(value_smallScale, sign);
  return smaller_scale;
}

// ф-я выравнивания степеней
int to_same_scale(s21_decimal_long *value_1, s21_decimal_long *value_2) {
  int scale1 = getScale_long(*value_1);
  int scale2 = getScale_long(*value_2);
  int new_scale = 0;
  if (scale2 > scale1)
    new_scale = scale_up(value_1, scale2, scale1);
  else if (scale1 > scale2)
    new_scale = scale_up(value_2, scale1, scale2);
  return new_scale;
}

int is_zero(s21_decimal_long dec) {
  int zero = 0;
  if (!dec.bits[0] && !dec.bits[1] && !dec.bits[2] && !dec.bits[3] &&
      !dec.bits[4] && !dec.bits[5] && !dec.bits[6])
    zero = 1;
  return zero;
}

void masCreat(unsigned int *arr, s21_decimal x) {
  int index = 0;
  for (int i = 0; i < 128; i++) {
    arr[index] = getBit(x, i);
    index++;
  }
}

int float_getScale(float value) {
  int res = 0;
  unsigned int fbits = *((unsigned int *)&value);
  int index = 31;
  for (unsigned int mask = 0x80000000; index > 22; mask >>= 1, index--) {
    if (!!(fbits & mask) && (index != 31))
      res += pow(2, index - 23);
  }
  return res - 127;
}

void shift(s21_decimal *x, int sign) {
  if (sign > 0) {
    x->bits[HIGHE] <<= 1;
    if (getBit(*x, 63))
      setBit(x, 64, 1);
    x->bits[MIDLE] <<= 1;
    if (getBit(*x, 31))
      setBit(x, 32, 1);
    x->bits[LOW] <<= 1;
  }

  if (sign < 0) {
    x->bits[LOW] >>= 1;
    if (getBit(*x, 32))
      setBit(x, 31, 1);
    x->bits[MIDLE] >>= 1;
    if (getBit(*x, 64))
      setBit(x, 63, 1);
    x->bits[HIGHE] >>= 1;
  }
}

void sub_from_big(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
                  int sign_val1, int sign_val2) {
  s21_decimal_long value_1_long = {0};
  s21_decimal_long value_2_long = {0};
  s21_decimal_long result_long = {0};
  from_decimal_to_long(value_1, &value_1_long);
  from_decimal_to_long(value_2, &value_2_long);
  from_decimal_to_long(*result, &result_long);
  setSign_long(&value_1_long, 0);
  setSign_long(&value_2_long, 0);
  if (s21_is_greater_or_equal(value_1, value_2)) {
    binary_sub(value_1_long, value_2_long, &result_long);
    if (sign_val1)
      setSign_long(&result_long, sign_val1);
  } else {
    binary_sub(value_1_long, value_2_long, &result_long);
    if (sign_val2)
      setSign_long(&result_long, sign_val2);
  }
}